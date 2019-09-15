#include "nyx/nyx.h"

#include "_font.h"

#include "_const.h"
#include "_glyph.h"
#include "_kerning.h"
#include "_mask.h"
#include "_sheet.h"

#define MAX_FONTS 256

#define FORMAT_VERSION 0

#define FLAG_MONOSPACED 0x01
#define FLAG_KERNING    0x02

static NYX_FONT *fonts[MAX_FONTS];

static int active_font = -1;

static float default_h_spacing = DEFAULT_FONT_H_SPACING;
static float default_v_spacing = DEFAULT_FONT_V_SPACING;

static void font_destroy(NYX_FONT *f) {
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
    f->glyphs = _glyphmap_init();
    if(!f->glyphs)
        goto fail;
    f->kernings = _kerningmap_init();
    if(!f->kernings)
        goto fail;
    return f;
fail:
    font_destroy(f);
    return 0;
}

static int register_font(NYX_FONT *f) {
    int idx;

    for(idx=0; idx<MAX_FONTS; ++idx)
        if(!fonts[idx])
            break;
    if(idx == MAX_FONTS)
        return -1;
    fonts[idx] = f;
    return idx;
}

static void set_default_spacing(NYX_FONT *f) {
    f->h_spacing = 1 + f->glyph_h * default_h_spacing;
    f->v_spacing = 1 + f->glyph_h * default_v_spacing;
}

static NYX_FONT *from_bitmap(const NYX_BITMAP *bitmap, int num_ranges, const uint32_t *range_pairs) {
    NYX_FONT *f = init_font();

    if(!f)
        return 0;
    if(_font_add_from_bitmap(f, bitmap, num_ranges, range_pairs))
    {
        font_destroy(f);
        return 0;
    }
    set_default_spacing(f);
    return f;
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
       nyx_file_read_upak(file, &v_spacing))
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
    if(_glyphmap_load(font->glyphs,
                      file,
                      font->glyph_w,
                      font->glyph_h,
                      font->monospaced))
        goto fail;
    if(font->kerning && _load_kernings(font->kernings, file))
        goto fail;
    return font;
fail:
    font_destroy(font);
    return 0;
}

static int save(const NYX_FONT *font, NYX_FILE *file) {
    uint8_t flags = 0;

    if(font->monospaced)
        flags |= FLAG_MONOSPACED;
    if(font->kerning)
        flags |= FLAG_KERNING;
    if(nyx_file_write_cstring(file, "nyxf", 4) ||
       nyx_file_write_u32(file, FORMAT_VERSION) ||
       nyx_file_write_u8(file, flags) ||
       nyx_file_write_u32(file, font->replacement) ||
       font->monospaced && nyx_file_write_upak(file, font->glyph_w - 1) ||
       nyx_file_write_upak(file, font->glyph_h - 1) ||
       nyx_file_write_upak(file, font->h_spacing) ||
       nyx_file_write_upak(file, font->v_spacing))
        return -1;
    if(_glyphmap_save(font->glyphs,
                      file,
                      font->glyph_w,
                      font->glyph_h,
                      font->monospaced))
        return -1;
    if(font->kerning && _save_kernings(font->kernings, file))
        return -1;
    return 0;
}

NYX_FONT *_get_active_font(void) {
    if(active_font < 0)
        return 0;
    return fonts[active_font];
}

int _font_mask_ptr(const NYX_FONT *font, uint32_t code, NYX_MASK *mask) {
    struct glyph *glyph = _glyphmap_get(font->glyphs, code);

    if(!glyph)
        return -1;
    mask->w = font->monospaced ? font->glyph_w : glyph->w;
    mask->h = font->glyph_h;
    mask->bits = glyph->bits;
    return 0;
}

int nyx_make_font(void) {
    NYX_FONT *f;
    int idx;

    f = init_font();
    if(!f)
        return -1;
    idx = register_font(f);
    if(idx < 0)
        font_destroy(f);
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
    idx = register_font(font);
    if(idx < 0)
        font_destroy(font);
    return idx;
}

int nyx_font_save(const char *path) {
    const NYX_FONT *font = _get_active_font();

    NYX_FILE *file;
    int err;

    if(!font)
        return -1;
    file = nyx_file_open(path, NYX_FILE_WRITE, NYX_FILE_BIT);
    if(!file)
        return -1;
    err = save(font, file);
    nyx_file_close(file);
    return err;
}

int nyx_font_import_from_bitmap(const NYX_BITMAP *bitmap, int num_ranges, const uint32_t *range_pairs) {
    NYX_FONT *f = from_bitmap(bitmap, num_ranges, range_pairs);
    int idx;

    if(!f)
        return -1;
    idx = register_font(f);
    if(idx < 0)
        font_destroy(f);
    return idx;
}

int nyx_font_import_ascii_from_bitmap(const NYX_BITMAP *bitmap) {
    return nyx_font_import_from_bitmap(bitmap, 1, ASCII_RANGE);
}

int nyx_font_import(const char *path, int num_ranges, uint32_t *range_pairs) {
    NYX_BITMAP *bitmap = nyx_load_bitmap(path);
    int idx;

    if(!bitmap)
        return -1;
    idx = nyx_font_import_from_bitmap(bitmap, num_ranges, range_pairs);
    nyx_destroy_bitmap(bitmap);
    return idx;
}

int nyx_font_import_ascii(const char *path) {
    return nyx_font_import(path, 1, ASCII_RANGE);
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
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    return f->glyph_h;
}

int nyx_font_h_spacing(void) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    return f->h_spacing;
}

int nyx_font_v_spacing(void) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    return f->v_spacing;
}

int nyx_set_font_h_spacing(int16_t h_spacing) {
    NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    f->h_spacing = h_spacing;
    return 0;
}

int nyx_set_font_v_spacing(int16_t v_spacing) {
    NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    f->v_spacing = v_spacing;
    return 0;
}

int nyx_set_font_spacing(int16_t spacing) {
    NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    f->h_spacing = f->v_spacing = spacing;
    return 0;
}

uint32_t nyx_replacement_glyph() {
    NYX_FONT *f = _get_active_font();

    if(!f)
        return 0;
    return f->replacement;
}

int nyx_set_replacement_glyph(uint32_t code) {
    NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    f->replacement = code;
    return 0;
}

int nyx_font_monospaced(void) {
    const NYX_FONT *f = _get_active_font();

    if(!f)
        return -1;
    return f->monospaced;
}
