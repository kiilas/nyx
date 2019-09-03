#include "nyx/nyx.h"

#include "_const.h"

#define MAX_FONTS 256

#define FORMAT_VERSION 0

#define FLAG_MONOSPACED 0x01
#define FLAG_KERNING    0x02

struct kerning_key {
    uint32_t left;
    uint32_t right;
};

struct kerning_data {
    int16_t  spacing;
};

struct glyph {
    uint16_t  w;
    void     *bits;
};

struct NYX_FONT {
    uint32_t replacement;
    uint16_t glyph_w;
    uint16_t glyph_h;
    int16_t  h_spacing;
    int16_t  v_spacing;
    bool     monospaced;
    bool     kerning;
    NYX_MAP *glyphs;
    NYX_MAP *kernings;
};

static NYX_FONT *fonts[MAX_FONTS];

static int active_font = -1;

static float default_h_spacing = DEFAULT_FONT_H_SPACING;
static float default_v_spacing = DEFAULT_FONT_V_SPACING;

static void destroy_font(NYX_FONT *f) {
    if(!f)
        return;
    nyx_destroy_map(f->glyphs);
    nyx_destroy_map(f->kernings);
    free(f);
}

static NYX_FONT *init_font(void) {
    NYX_FONT *f;

    f = calloc(1, sizeof(NYX_FONT));
    if(!f)
        return 0;
    f->glyphs = nyx_init_map(4, sizeof(struct glyph));
    if(!f->glyphs)
        goto fail;
    f->kernings = nyx_init_map(sizeof(struct kerning_key),
                               sizeof(struct kerning_data));
    if(!f->kernings)
        goto fail;
    return f;
fail:
    destroy_font(f);
    return 0;
}

static int next_glyph(const NYX_BITMAP *bitmap, uint32_t border_color, int *x, int *y) {
    do
        if(++*x == nyx_get_bitmap_width(bitmap))
        {
            if(++*y == nyx_get_bitmap_height(bitmap))
                return 1;
            *x = 0;
        }
    while(nyx_get_bitmap_pixel_unsafe(bitmap, *x, *y) == border_color ||
          *x && nyx_get_bitmap_pixel_unsafe(bitmap, *x-1, *y) != border_color ||
          *y && nyx_get_bitmap_pixel_unsafe(bitmap, *x, *y-1) != border_color);
    return 0;
}

static int add_glyph_data(NYX_FONT *f, uint32_t code, int w, int h, void *bits) {
    struct glyph glyph;
    uint8_t key[4];

    if(!f)
        return -1;
    if(h != f->glyph_h && f->glyph_h)
        return -1;
    f->glyph_h = h;
    if(f->monospaced)
    {
        if(w != f->glyph_w && f->glyph_w)
            return -1;
        f->glyph_w = w;
    }
    glyph.w = w;
    glyph.bits = bits;
    nyx_u32_to_bytes(key, code);
    if(nyx_map_insert(f->glyphs, key, &glyph))
        return -1;
    return 0;
}

static void *glyph_bits(const NYX_BITMAP *bitmap, int x, int y, int w, int h) {
    void *bits;
    int cx;
    int cy;

    bits = malloc((w*h + 7) / 8);
    if(!bits)
        return 0;
    for(cy=0; cy<h; ++cy)
        for(cx=0; cx<w; ++cx)
        {
            bool value = nyx_get_bitmap_pixel_alpha_unsafe(bitmap, x+cx, y+cy) == 0xff;

            nyx_set_bit_unsafe(bits, cx+cy*w, value);
        }
    return bits;
}

static int add_glyph_rect(NYX_FONT *f, const NYX_BITMAP *bitmap, int x, int y, int w, int h, uint32_t code) {
    void *bits;
    int res;

    if(!f)
        return -1;
    if(!bitmap)
        return -1;
    bits = glyph_bits(bitmap, x, y, w, h);
    if(!bits)
        return -1;
    res = add_glyph_data(f, code, w, h, bits);
    if(res)
        free(bits);
    return res;
}

static int add_glyph(NYX_FONT *f, const NYX_BITMAP *bitmap, uint32_t border_color, int x, int y, uint32_t code) {
    int w = 1;
    int h = 1;
    int px;
    int py;

    if(!f)
        return -1;
    if(!bitmap)
        return -1;
    for(px=x; px<nyx_get_bitmap_width(bitmap); ++px)
        if(nyx_get_bitmap_pixel_unsafe(bitmap, px, y) == border_color)
            break;
    w = px - x;
    if(w <= 0)
        return -1;
    for(py=y; py<nyx_get_bitmap_height(bitmap); ++py)
        if(nyx_get_bitmap_pixel_unsafe(bitmap, x, py) == border_color)
            break;
    h = py - y;
    if(h <= 0)
        return -1;
    return add_glyph_rect(f, bitmap, x, y, w, h, code);
}

static int add_glyphs(NYX_FONT *f, const NYX_BITMAP *bitmap, uint32_t from) {
    uint32_t border_color;
    uint32_t code = from;
    int x;
    int y;

    if(!f)
        return -1;
    if(nyx_get_bitmap_pixel(bitmap, 0, 0, &border_color))
        return -1;
    x = 0;
    y = 0;
    while(!next_glyph(bitmap, border_color, &x, &y))
        if(add_glyph(f, bitmap, border_color, x, y, code++))
            return -1;
    return code - from;
}

static void set_default_spacing(NYX_FONT *f) {
    f->h_spacing = 1 + f->glyph_h * default_h_spacing;
    f->v_spacing = 1 + f->glyph_h * default_v_spacing;
}

static NYX_FONT *from_bitmap(const NYX_BITMAP *bitmap) {
    NYX_FONT *f;

    f = init_font();
    if(!f)
        return 0;
    if(add_glyphs(f, bitmap, 0x20) < 0)
    {
        destroy_font(f);
        return 0;
    }
    set_default_spacing(f);
    return f;
}

static NYX_FONT *get_active_font(void) {
    if(active_font < 0)
        return 0;
    return fonts[active_font];
}

static int load_glyph(NYX_FONT *font, NYX_FILE *file, uint32_t *previous, bool first) {
    uint32_t code;
    int64_t offset;
    int64_t width;
    void *bits = 0;

    if(nyx_file_read_upak(file, &offset))
        return -1;
    if(font->monospaced)
        width = font->glyph_w;
    else if(nyx_file_read_upak(file, &width))
        return -1;
    if(width)
    {
        bits = malloc((width * font->glyph_h + 7) / 8);
        if(!bits)
            return -1;
        if(nyx_file_read_bits(file, bits, width * font->glyph_h))
            goto fail;
    }
    code = first ? offset : *previous+offset+1;
    *previous = code;
    if(add_glyph_data(font, code, width, font->glyph_h, bits))
        goto fail;
    return 0;
fail:
    free(bits);
    return -1;
}

static NYX_FONT *load(NYX_FILE *file) {
    NYX_FONT *font;
    char buf[4];
    uint8_t flags;
    uint32_t u32;
    int64_t glyph_w;
    int64_t glyph_h;
    int64_t h_spacing;
    int64_t v_spacing;
    int64_t num_glyphs;
    uint32_t previous;
    uint32_t idx;

    if(nyx_file_read(file, buf, 4))
        return 0;
    if(memcmp(buf, "nyxf", 4))
        return 0;
    if(nyx_file_read_u32(file, &u32))
        return 0;
    if(u32 != FORMAT_VERSION)
        return 0;
    font = init_font();
    if(!font)
        return 0;
    if(nyx_file_read_u8(file, &flags) ||
       nyx_file_read_u32(file, &font->replacement) ||
       flags & FLAG_MONOSPACED && nyx_file_read_upak(file, &glyph_w) ||
       nyx_file_read_upak(file, &glyph_h) ||
       nyx_file_read_upak(file, &h_spacing) ||
       nyx_file_read_upak(file, &v_spacing) ||
       nyx_file_read_upak(file, &num_glyphs))
        goto fail;
    if(flags & FLAG_MONOSPACED)
    {
        font->monospaced = true;
        font->glyph_w = glyph_w + 1;
    }
    if(flags & FLAG_KERNING)
        font->kerning = true;
    font->glyph_h = glyph_h + 1;
    font->h_spacing = h_spacing;
    font->v_spacing = v_spacing;
    for(idx=0; idx<num_glyphs; ++idx)
        if(load_glyph(font, file, &previous, idx == 0))
            goto fail;
    return font;
fail:
    destroy_font(font);
    return 0;
}

static struct glyph *get_glyph(const NYX_FONT *f, uint32_t code) {
    uint8_t key[4];

    if(!f)
        return 0;
    nyx_u32_to_bytes(key, code);
    return nyx_map_get(f->glyphs, key);
}

int nyx_register_font(NYX_FONT *f) {
    int idx;

    for(idx=0; idx<MAX_FONTS; ++idx)
        if(!fonts[idx])
            break;
    if(idx == MAX_FONTS)
        return -1;
    fonts[idx] = f;
    return idx;
}

int nyx_make_font(void) {
    NYX_FONT *f;
    int idx;

    f = init_font();
    if(!f)
        return -1;
    idx = nyx_register_font(f);
    if(idx < 0)
        destroy_font(f);
    return idx;
}

int nyx_font_load(const char *path) {
    NYX_FILE *file;
    NYX_FONT *font;
    int idx;

    file = nyx_file_open(path, NYX_FILE_READ, NYX_FILE_BIT);
    if(!file)
        return -1;
    font = load(file);
    nyx_file_close(file);
    if(!font)
        return -1;
    idx = nyx_register_font(font);
    if(idx < 0)
        destroy_font(font);
    return idx;
}

int nyx_font_save(const char *path) {
    const NYX_FONT *f = get_active_font();

    NYX_FILE *file;
    uint8_t flags;
    uint32_t num_glyphs;
    uint32_t previous;
    uint32_t idx;

    if(!f)
        return -1;
    file = nyx_file_open(path, NYX_FILE_WRITE, NYX_FILE_BIT);
    if(!file)
        return -1;
    // TODO clean this shit up
    nyx_file_write_cstring(file, "nyxf", 4);
    nyx_file_write_u32(file, FORMAT_VERSION);
    flags = 0;
    if(f->monospaced)
        flags |= FLAG_MONOSPACED;
    if(f->kerning)
        flags |= FLAG_KERNING;
    nyx_file_write_u8(file, flags);
    nyx_file_write_u32(file, f->replacement);
    if(f->monospaced)
        nyx_file_write_upak(file, f->glyph_w - 1);
    nyx_file_write_upak(file, f->glyph_h - 1);
    nyx_file_write_upak(file, f->h_spacing);
    nyx_file_write_upak(file, f->v_spacing);
    nyx_font_num_glyphs(&num_glyphs);
    nyx_file_write_upak(file, num_glyphs);
    for(idx=0; idx<num_glyphs; ++idx)
    {
        uint32_t code;
        const void *bits;
        int width;

        nyx_font_code_by_index(idx, &code);
        bits = nyx_glyph_bits(code);
        width = nyx_glyph_width(code);
        if(idx)
            nyx_file_write_upak(file, code - previous - 1);
        else
            nyx_file_write_upak(file, code);
        if(!f->monospaced)
            nyx_file_write_upak(file, width);
        nyx_file_write_bits(file, bits, width * f->glyph_h);
        previous = code;
    }
    nyx_file_close(file);
    return 0;
}

int nyx_import_font(const char *path) {
    NYX_BITMAP *bitmap;
    int idx;

    bitmap = nyx_load_bitmap(path);
    if(!bitmap)
        return -1;
    idx = nyx_import_font_from_bitmap(bitmap);
    nyx_destroy_bitmap(bitmap);
    return idx;
}

int nyx_import_font_from_bitmap(const NYX_BITMAP *bitmap) {
    NYX_FONT *f;

    f = from_bitmap(bitmap);
    if(!f)
        return -1;
    return nyx_register_font(f);
}

int nyx_select_font(int idx) {
    if(idx < 0 || idx >= MAX_FONTS)
        return -1;
    if(!fonts[idx])
        return -1;
    active_font = idx;
    return 0;
}

int nyx_active_font(void) {
    return active_font;
}

int nyx_font_height(void) {
    const NYX_FONT *f = get_active_font();

    if(!f)
        return -1;
    return f->glyph_h;
}

int nyx_font_h_spacing(void) {
    const NYX_FONT *f = get_active_font();

    if(!f)
        return -1;
    return f->h_spacing;
}

int nyx_font_v_spacing(void) {
    const NYX_FONT *f = get_active_font();

    if(!f)
        return -1;
    return f->v_spacing;
}

int nyx_set_font_h_spacing(int16_t h_spacing) {
    NYX_FONT *f = get_active_font();

    if(!f)
        return -1;
    f->h_spacing = h_spacing;
    return 0;
}

int nyx_set_font_v_spacing(int16_t v_spacing) {
    NYX_FONT *f = get_active_font();

    if(!f)
        return -1;
    f->v_spacing = v_spacing;
    return 0;
}

int nyx_set_font_spacing(int16_t spacing) {
    NYX_FONT *f = get_active_font();

    if(!f)
        return -1;
    f->h_spacing = f->v_spacing = spacing;
    return 0;
}

uint32_t nyx_replacement_glyph() {
    NYX_FONT *f = get_active_font();

    if(!f)
        return 0;
    return f->replacement;
}

int nyx_set_replacement_glyph(uint32_t code) {
    NYX_FONT *f = get_active_font();

    if(!f)
        return -1;
    f->replacement = code;
    return 0;
}

int nyx_glyph_width(uint32_t code) {
    const NYX_FONT *f = get_active_font();
    const struct glyph *g;

    if(!f)
        return -1;
    if(f->monospaced)
        return f->glyph_w;
    g = get_glyph(f, code);
    if(!g)
    {
        g = get_glyph(f, nyx_replacement_glyph());
        if(!g)
            return -1;
    }
    return g->w;
}

const void *nyx_glyph_bits(uint32_t code) {
    const NYX_FONT *f = get_active_font();
    const struct glyph *g;

    if(!f)
        return 0;
    g = get_glyph(f, code);
    if(!g)
        return 0;
    return g->bits;
}

int nyx_font_num_glyphs(uint32_t *num_glyphs) {
    const NYX_FONT *f = get_active_font();

    size_t size;

    if(!f)
        return -1;
    if(nyx_map_size(f->glyphs, &size))
        return -1;
    *num_glyphs = size;
    return 0;
}

int nyx_font_code_by_index(uint32_t idx, uint32_t *code) {
    const NYX_FONT *f = get_active_font();
    const void *key;
    
    if(!f)
        return -1;
    key = nyx_map_key_by_index(f->glyphs, idx);
    if(!key)
        return -1;
    *code = nyx_bytes_to_u32(key);
    return 0;
}
