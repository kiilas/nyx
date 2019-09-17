#include "nyx/nyx.h"

#include "_font.h"
#include "_glyph.h"

#include <assert.h>

static int pair_set(NYX_MAP *kernings, uint32_t prev, uint32_t next, int16_t offset) {
    uint8_t key[8];
    uint8_t value[2];

    nyx_u32_to_bytes(key, prev);
    nyx_u32_to_bytes(key+4, next);
    if(!offset)
    {
        nyx_map_remove(kernings, key);
        return 0;
    }
    nyx_i16_to_bytes(value, offset);
    return nyx_map_insert(kernings, key, value);
}

static size_t num_pairs(const NYX_MAP *kernings) {
    return nyx_map_size(kernings);
}

static NYX_KERNING_PAIR by_index(const NYX_MAP *kernings, size_t idx) {
    NYX_KERNING_PAIR pair;
    const uint8_t *key;
    const uint8_t *value;

    key = nyx_map_key_by_index_unsafe(kernings, idx);
    value = nyx_map_value_by_index_unsafe(kernings, idx);
    pair.prev = nyx_bytes_to_u32(key);
    pair.next = nyx_bytes_to_u32(key + 4);
    pair.offset = nyx_bytes_to_i16(value);
    return pair;
}

static int load_kerning(NYX_MAP *kernings, NYX_FILE *file, uint32_t *last_prev, uint32_t *last_next, bool first) {
    int64_t packed_prev;
    int64_t packed_next;
    int64_t packed_offset;
    uint32_t prev;
    uint32_t next;
    int16_t offset;

    if(nyx_file_read_upak(file, &packed_prev))
        return -1;
    prev = first ? packed_prev : *last_prev+packed_prev;
    if(!first && *last_prev == prev)
    {
        if(nyx_file_read_upak(file, &packed_next))
            return -1;
        next = *last_next + packed_next + 1;
    }
    else
    {
        if(nyx_file_read_ipak(file, &packed_next))
            return -1;
        next = prev - packed_next;
    }
    if(nyx_file_read_ipak(file, &packed_offset))
        return -1;
    if(packed_offset >= 0)
        offset = packed_offset + 1;
    else
        offset = packed_offset;
    if(pair_set(kernings, prev, next, offset))
        return -1;
    *last_prev = prev;
    *last_next = next;
    return 0;
}

static int save_pair(NYX_FILE *file, const NYX_KERNING_PAIR pair, uint32_t *last_prev, uint32_t *last_next, bool first) {
    int packed_prev;
    int packed_offset;

    assert(pair.offset);
    packed_prev = first ? pair.prev : pair.prev - *last_prev;
    if(nyx_file_write_upak(file, packed_prev))
        return -1;
    if(first || pair.prev != *last_prev)
    {
        if(nyx_file_write_ipak(file, (int64_t)pair.prev - pair.next))
            return -1;
    }
    else if(nyx_file_write_upak(file, pair.next - *last_next - 1))
        return -1;
    packed_offset = pair.offset > 0 ? pair.offset - 1 : pair.offset;
    if(nyx_file_write_ipak(file, packed_offset))
        return -1;
    *last_prev = pair.prev;
    *last_next = pair.next;
    return 0;
}

NYX_MAP *_kerningmap_init(void) {
    return nyx_init_map(8, 2);
}

int _load_kernings(NYX_MAP *kernings, NYX_FILE *file) {
    int64_t num_kernings;
    uint32_t last_prev;
    uint32_t last_next;
    int64_t idx;

    if(nyx_file_read_upak(file, &num_kernings))
        return -1;
    for(idx=0; idx<num_kernings; ++idx)
        if(load_kerning(kernings, file, &last_prev, &last_next, idx == 0))
            return -1;
    return 0;
}

int _save_kernings(const NYX_MAP *kernings, NYX_FILE *file) {
    size_t pairs = num_pairs(kernings);
    size_t idx;
    uint32_t last_prev;
    uint32_t last_next;

    if(nyx_file_write_upak(file, pairs))
        return -1;
    for(idx=0; idx<pairs; ++idx)
    {
        NYX_KERNING_PAIR pair = by_index(kernings, idx);

        if(save_pair(file, pair, &last_prev, &last_next, idx == 0))
            return -1;
    }
    return 0;
}

int nyx_font_kerning(void) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    return f->kerning;
}

int nyx_font_set_kerning(int kerning) {
    NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    if(kerning && f->monospaced)
        return -1;
    f->kerning = kerning;
    return 0;
}

int nyx_font_kerning_clear(void) {
    NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    nyx_map_clear(f->kernings);
    return 0;
}

int nyx_font_kerning_num_pairs(size_t *pairs)
{
    NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    *pairs = f->kerning ? num_pairs(f->kernings) : 0;
    return 0;
}

int nyx_font_kerning_pair(uint32_t prev, uint32_t next) {
    const NYX_FONT *f = _get_active_font();

    uint8_t key[8];
    const void *value;

    if(!f)
        return 0;
    if(!f->kerning)
        return 0;
    nyx_u32_to_bytes(key, prev);
    nyx_u32_to_bytes(key+4, next);
    value = nyx_map_get(f->kernings, key);
    if(!value)
        return 0;
    return nyx_bytes_to_i16(value);
}

int nyx_font_kerning_pair_set(uint32_t prev, uint32_t next, int16_t offset) {
    NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    if(!f->kerning)
        return -1;
    return pair_set(f->kernings, prev, next, offset);
}

int nyx_font_kerning_pair_by_index(size_t idx, NYX_KERNING_PAIR *pair) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    if(!f->kerning)
        return -1;
    if(idx >= num_pairs(f->kernings))
        return -1;
    *pair = by_index(f->kernings, idx);
    return 0;
}

static int front_gap(const struct glyph *g, int y) {
    int x;

    for(x=g->w; x>0; --x)
        if(_glyph_bit(g, x-1, y))
            break;
    return g->w - x;
}

static int back_gap(const struct glyph *g, int y) {
    int x;

    for(x=0; x<g->w; ++x)
        if(_glyph_bit(g, x, y))
            break;
    return x;
}

int nyx_font_kerning_auto_pair(uint32_t prev, uint32_t next) {
    const NYX_FONT *f = _get_active_font();
    const struct glyph *prev_glyph;
    const struct glyph *next_glyph;
    int font_height;
    int max_gap;
    int offset;
    int y;

    if(!f)
        return -1;
    if(!f->kerning)
        return -1;
    prev_glyph = _glyphmap_get(f->glyphs, prev);
    next_glyph = _glyphmap_get(f->glyphs, next);
    if(!prev_glyph || !next_glyph)
        return -1;
    font_height = f->glyph_h;
    max_gap = prev_glyph->w + next_glyph->w;
    for(y=0; y<font_height; ++y)
    {
        int gap = front_gap(prev_glyph, y) + back_gap(next_glyph, y);

        if(gap < max_gap)
            max_gap = gap;
    }
    offset = max_gap>f->h_spacing ? f->h_spacing : max_gap;
    return pair_set(f->kernings, prev, next, offset);
}

int nyx_font_kerning_auto_range(uint32_t prev_min, uint32_t prev_max, uint32_t next_min, uint32_t next_max) {
    uint32_t prev;
    uint32_t next;

    for(prev=prev_min; prev<=prev_max; ++prev)
    {
        if(!nyx_glyph_exists(prev))
            continue;
        for(next=next_min; next<=next_max; ++next)
        {
            if(!nyx_glyph_exists(next))
                continue;
            if(nyx_font_kerning_auto_pair(prev, next))
                return -1;
        }
    }
    return 0;
}

int nyx_font_kerning_auto_ranges(int num_ranges, const uint32_t *range_pairs) {
    int prev;
    int next;

    for(prev=0; prev<num_ranges; ++prev)
        for(next=0; next<num_ranges; ++next)
        {
            uint32_t prev_min = range_pairs[2*prev];
            uint32_t prev_max = range_pairs[2*prev + 1];
            uint32_t next_min = range_pairs[2*next];
            uint32_t next_max = range_pairs[2*next + 1];
            if(nyx_font_kerning_auto_range(prev_min,
                                           prev_max,
                                           next_min,
                                           next_max))
                return -1;
        }
    return 0;
}
