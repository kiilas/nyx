#ifndef NYX__FONT_H
#define NYX__FONT_H

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

NYX_FONT *_get_active_font(void);

int _font_mask_ptr(const NYX_FONT *font, uint32_t code, NYX_MASK *mask);

#endif
