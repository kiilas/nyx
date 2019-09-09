#include "nyx/nyx.h"

#include "_font.h"
#include "_mask.h"
#include "_pipeline.h"
#include "_text.h"
#include "_unicode.h"

struct text_stream _text_stream(int32_t left, int32_t top, int wrap, bool multiline) {
    return (struct text_stream) {.left=left,
                                 .top=top, 
                                 .wrap=wrap,
                                 .multiline=multiline};
}

int _text_draw_char(int32_t x, int32_t y, uint32_t code, const NYX_FONT *font, NYX_COLOR color) {
    NYX_VECTOR v;
    NYX_MASK mask;
    
    if(_font_mask_ptr(font, code, &mask) &&
       _font_mask_ptr(font, font->replacement, &mask))
        return -1;
    v = nyx_vector(x, y);
    return _pipeline_mask(v, mask, color);
}

int _text_draw_stream_newline(struct text_stream *stream, const NYX_FONT *font) {
    if(!stream->multiline)
        return -1;
    stream->x = 0;
    stream->y += font->glyph_h + font->v_spacing;
    stream->prev = 0;
    return 0;
}

int _text_stream_draw_char(struct text_stream *stream, const NYX_FONT *font, uint32_t code, NYX_COLOR color) {
    NYX_VECTOR v;
    NYX_MASK mask;

    if(code == '\n')
        return _text_draw_stream_newline(stream, font);
    // temporary workaround for control codes
    if(code < 0x20)
        return 0;
    if(_font_mask_ptr(font, code, &mask))
    {
        if(_font_mask_ptr(font, font->replacement, &mask))
            return -1;
        code = font->replacement;
    }
    if(stream->prev)
        stream->x -= nyx_font_kerning_pair(stream->prev, code);
    if(stream->wrap && stream->x+mask.w > stream->wrap && stream->x)
        _text_draw_stream_newline(stream, font);
    v = nyx_vector(stream->left+stream->x, stream->top+stream->y);
    if(_pipeline_mask(v, mask, color))
        return -1;
    stream->x += mask.w + font->h_spacing;
    stream->prev = code;
    return 0;
}

int _text_stream_draw_string(struct text_stream *stream, const NYX_FONT *font, bool nullterm, const char *str, size_t size, NYX_COLOR color) {
    size_t idx = 0;

    while(idx < size)
    {
        uint32_t code;

        if(_unicode_decode(str, size, &idx, &code))
            return -1;
        if(!code && nullterm)
            break;
        if(_text_stream_draw_char(stream, font, code, color))
            return -1;
    }
    return 0;
}

int _text_draw_string(const NYX_FONT *font, int32_t x, int32_t y, int wrap, bool multiline, bool nullterm, const char *str, size_t size, NYX_COLOR color) {
    struct text_stream stream = _text_stream(x, y, wrap, multiline);

    return _text_stream_draw_string(&stream, font, nullterm, str, size, color);
}
