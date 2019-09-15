#include "nyx/nyx.h"

#include "_font.h"
#include "_glyph.h"
#include "_mask.h"
#include "_sheet.h"

static struct glyph *glyph_init(int width, int height) {
    struct glyph *glyph = calloc(1, sizeof(struct glyph));

    if(!glyph)
        return 0;
    if(width)
    {
        glyph->bits = nyx_bits_alloc(width * height);
        if(!glyph->bits)
            goto fail;
        glyph->w = width;
    }
    return glyph;
fail:
    free(glyph);
    return 0;
}

static void glyph_destroy(struct glyph *glyph) {
    if(glyph->w)
        free(glyph->bits);
    free(glyph);
}

static void glyph_destroy_keep_bits(struct glyph *glyph) {
    free(glyph);
}

static struct glyph *glyph_load(NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced) {
    struct glyph *glyph;
    int64_t width;

    if(monospaced)
        width = glyph_w;
    else if(nyx_file_read_upak(file, &width))
        return 0;
    glyph = glyph_init(width, glyph_h);
    if(!glyph)
        return 0;
    if(width && nyx_file_read_bits(file, glyph->bits, width * glyph_h))
    {
        glyph_destroy(glyph);
        return 0;
    }
    return glyph;
}

static int glyph_save(const struct glyph *glyph, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced) {
    int width = monospaced ? glyph_w : glyph->w;

    if(!monospaced && nyx_file_write_upak(file, width) ||
       nyx_file_write_bits(file, glyph->bits, width * glyph_h))
        return -1;
    return 0;
}

static uint32_t glyphmap_size(const NYX_MAP *glyphs) {
    return nyx_map_size(glyphs);
}

static struct glyph *glyphmap_by_index(const NYX_MAP *glyphs, uint32_t idx) {
    return nyx_map_value_by_index(glyphs, idx);
}

static uint32_t glyphmap_code_by_index(const NYX_MAP *glyphs, uint32_t idx) {
    const void *key;

    key = nyx_map_key_by_index(glyphs, idx);
    return nyx_bytes_to_u32(key);
}

static int glyphmap_add_glyph(NYX_MAP *glyphs, uint32_t code, struct glyph *glyph) {
    uint8_t key[4];

    nyx_u32_to_bytes(key, code);
    return nyx_map_insert(glyphs, key, glyph);
}

static int glyphmap_add(NYX_MAP *glyphs, uint32_t code, uint16_t w, void *bits) {
    struct glyph glyph;

    glyph.w = w;
    glyph.bits = bits;
    return glyphmap_add_glyph(glyphs, code, &glyph);
}

static int glyphmap_exists(const NYX_MAP *glyphs, uint32_t code) {
    return _glyphmap_get(glyphs, code) != 0;
}

static int glyphmap_width(const NYX_MAP *glyphs, uint32_t code) {
    const struct glyph *g = _glyphmap_get(glyphs, code);

    return g ? g->w : -1;
}

static const void *glyphmap_bits(const NYX_MAP *glyphs, uint32_t code) {
    const struct glyph *g = _glyphmap_get(glyphs, code);

    return g ? g->bits : 0;
}

int _glyph_bit(const struct glyph *glyph, int x, int y) {
    return nyx_get_bit_unsafe(glyph->bits, x + y*glyph->w);
}

NYX_MAP *_glyphmap_init(void) {
    return nyx_init_map(4, sizeof(struct glyph));
}

int _glyphmap_load(NYX_MAP *glyphs, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced) {
    int64_t size;
    uint32_t idx;
    uint32_t previous;

    if(nyx_file_read_upak(file, &size))
        return -1;
    for(idx=0; idx<size; ++idx)
    {
        struct glyph *glyph;
        int64_t offset;
        uint32_t code;

        if(nyx_file_read_upak(file, &offset))
            return -1;
        glyph = glyph_load(file, glyph_w, glyph_h, monospaced);
        if(!glyph)
            return -1;
        code = idx ? previous+offset+1 : offset;
        previous = code;
        if(glyphmap_add_glyph(glyphs, code, glyph))
        {
            glyph_destroy(glyph);
            return -1;
        }
        glyph_destroy_keep_bits(glyph);
    }
    return 0;
}

int _glyphmap_save(const NYX_MAP *glyphs, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced) {
    uint32_t size = glyphmap_size(glyphs);
    uint32_t idx;
    uint32_t previous;

    if(nyx_file_write_upak(file, size))
        return -1;
    for(idx=0; idx<size; ++idx)
    {
        const struct glyph *glyph = glyphmap_by_index(glyphs, idx);
        uint32_t code = glyphmap_code_by_index(glyphs, idx);
        uint32_t packed_prev;

        packed_prev = idx ? code-previous-1 : code;
        if(nyx_file_write_upak(file, packed_prev))
            return -1;
        if(glyph_save(glyph, file, glyph_w, glyph_h, monospaced))
            return -1;
        previous = code;
    }
    return 0;
}

struct glyph *_glyphmap_get(const NYX_MAP *glyphs, uint32_t code) {
    uint8_t key[4];

    nyx_u32_to_bytes(key, code);
    return nyx_map_get(glyphs, key);
}

static int add_glyph(NYX_FONT *f, uint32_t code, int w, int h, void *bits) {
    if(h != f->glyph_h && f->glyph_h)
        return -1;
    if(f->monospaced)
    {
        if(w != f->glyph_w && f->glyph_w)
            return -1;
        f->glyph_w = w;
    }
    f->glyph_h = h;
    return glyphmap_add(f->glyphs, code, w, bits);
}

static int add_glyph_mask(NYX_FONT *f, uint32_t code, NYX_MASK *mask) {
    void *bits = nyx_mask_bits_copy(mask);

    if(!bits)
        return -1;
    if(add_glyph(f, code, mask->w, mask->h, bits))
    {
        free(bits);
        return -1;
    }
    return 0;
}

int _font_add_from_bitmap(NYX_FONT *font, const NYX_BITMAP *bitmap, int num_ranges, const uint32_t *range_pairs) {
    uint32_t border_color;
    int range;
    int x = 0;
    int y = 0;

    if(nyx_get_bitmap_pixel(bitmap, 0, 0, &border_color))
        return -1;
    for(range=0; range<num_ranges; ++range)
    {
        uint32_t from = range_pairs[2*range];
        uint32_t to = range_pairs[2*range + 1];
        uint32_t code;

        for(code=from; code<=to; ++code)
        {
            NYX_MASK *mask = _sheet_mask(bitmap, border_color, &x, &y);
            int err;

            if(!mask)
                return -1;
            err = add_glyph_mask(font, code, mask);
            nyx_mask_destroy(mask);
            if(err)
                return -1;
        }
    }
    return 0;
}

int nyx_font_glyphs_import_from_bitmap(const NYX_BITMAP *bitmap, int num_ranges, const uint32_t *range_pairs) {
    NYX_FONT *font = _get_active_font();

    if(!font)
        return -1;
    return _font_add_from_bitmap(font, bitmap, num_ranges, range_pairs);
}

int nyx_font_glyphs_import_ascii_from_bitmap(const NYX_BITMAP *bitmap) {
    return nyx_font_glyphs_import_from_bitmap(bitmap, 1, ASCII_RANGE);
}

int nyx_font_glyphs_import(const char *path, int num_ranges, const uint32_t *range_pairs) {
    NYX_BITMAP *bitmap = nyx_load_bitmap(path);
    int err;

    if(!bitmap)
        return -1;
    err = nyx_font_glyphs_import_from_bitmap(bitmap, num_ranges, range_pairs);
    nyx_destroy_bitmap(bitmap);
    return err;
}

int nyx_font_glyphs_import_ascii(const char *path) {
    return nyx_font_glyphs_import(path, 1, ASCII_RANGE);
}

int nyx_glyph_exists(uint32_t code) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return 0;
    return glyphmap_exists(f->glyphs, code);
}

int nyx_glyph_width(uint32_t code) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    if(f->monospaced)
        return f->glyph_w;
    return glyphmap_width(f->glyphs, code);
}

const void *nyx_glyph_bits(uint32_t code) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return 0;
    return glyphmap_bits(f->glyphs, code);
}

int nyx_font_num_glyphs(uint32_t *num_glyphs) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    *num_glyphs = glyphmap_size(f->glyphs);
    return 0;
}

int nyx_font_code_by_index(uint32_t idx, uint32_t *code) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    *code = glyphmap_code_by_index(f->glyphs, idx);
    return 0;
}
