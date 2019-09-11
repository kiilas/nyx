// TODO this code is a spaghetti mess, clean it up

#include "nyx/nyx.h"

#include "_const.h"
#include "_font.h"
#include "_mask.h"
#include "_pipeline.h"
#include "_text.h"
#include "_unicode.h"

struct word_buf_item {
    int32_t  x;
    uint32_t code;
};

static const NYX_FONT *stream_font;
static NYX_COLOR       stream_color;
static int32_t         stream_left;
static int32_t         stream_top;
static int32_t         stream_x;
static int32_t         stream_y;
static bool            stream_multiline;
static int             stream_wrap;
static bool            stream_word_wrap;
static uint32_t        stream_prev;

static int     word_buf_size;
static struct  word_buf_item word_buf[TEXT_WORD_BUF_SIZE];
static bool    word_buf_overflow;

int _text_draw_char(int32_t x, int32_t y, uint32_t code, const NYX_FONT *font, NYX_COLOR color) {
    NYX_VECTOR v;
    NYX_MASK mask;
    
    if(_font_mask_ptr(font, code, &mask) &&
       _font_mask_ptr(font, font->replacement, &mask))
        return -1;
    v = nyx_vector(x, y);
    return _pipeline_mask(v, mask, color);
}

static void reset_stream(void) {
    stream_x = 0;
    stream_y = 0;
    stream_prev = 0;
    word_buf_size = 0;
    word_buf_overflow = false;
}

static void word_buf_clear(void) {
    word_buf_size = 0;
    word_buf_overflow = false;
}

static int word_buf_flush(void) {
    int i;

    for(i=0; i<word_buf_size; ++i)
        if(_text_draw_char(stream_left + word_buf[i].x,
                           stream_top + stream_y,
                           word_buf[i].code,
                           stream_font,
                           stream_color))
            return -1;
    word_buf_clear();
    return 0;
}

static int wrap(bool *wrapped) {
    int32_t buf_x = word_buf_size ? word_buf[0].x : stream_x;

    if(!buf_x && word_buf_size)
    {
        // flush buffer
        if(word_buf_flush())
            return -1;

        // new line
        stream_x = 0;
        stream_y += stream_font->glyph_h + stream_font->v_spacing;
        stream_prev = 0;
        *wrapped = true;
    }
    else if(buf_x)
    {
        int i;

        for(i=0; i<word_buf_size; ++i)
            word_buf[i].x -= buf_x;

        // new line
        stream_x -= buf_x;
        stream_y += stream_font->glyph_h + stream_font->v_spacing;
        stream_prev = 0;
        *wrapped = true;
    }
    else
        *wrapped = false;
    return 0;
}

static int stream_newline(void) {
    if(!stream_multiline)
        return -1;
    stream_x = 0;
    stream_y += stream_font->glyph_h + stream_font->v_spacing;
    stream_prev = 0;
    return 0;
}

static int stream_draw_newline(void) {
    if(word_buf_flush())
        return -1;
    if(stream_newline())
        return -1;
    return 0;
}

static int stream_draw_char(uint32_t code) {
    NYX_MASK mask;
    bool wrapped = false;

    if(code == '\n')
        return stream_draw_newline();

    // temporary workaround for control codes
    if(code < 0x20)
    {
        if(word_buf_flush())
            return -1;
        return 0;
    }

    // get actual code to be rendered
    if(_font_mask_ptr(stream_font, code, &mask))
    {
        if(_font_mask_ptr(stream_font, stream_font->replacement, &mask))
            return -1;
        code = stream_font->replacement;
    }

    // apply kerning
    if(stream_prev)
        stream_x -= nyx_font_kerning_pair(stream_prev, code);

    // if word wrap and non-word char, flush buffer and reset
    if(stream_word_wrap && !_unicode_is_word_char(code) && word_buf_flush())
        return -1;

    // if no space to draw char, move to new line unless we're starting
    // from x=0 and there's no buffer to flush
    if(stream_wrap && stream_x+mask.w > stream_wrap && wrap(&wrapped))
        return -1;

    // if we just wrapped, we skip whitespace
    if(wrapped && _unicode_is_whitespace(code))
        return 0;

    // if word buffer full, flush it and mark overflow
    if(word_buf_size == TEXT_WORD_BUF_SIZE)
    {
        if(word_buf_flush())
            return -1;
        word_buf_overflow = true;
    }

    // determine if we push to word buffer or draw
    // if a word-character and we word-wrap, push to buffer unless overflow
    if(stream_word_wrap && !word_buf_overflow && _unicode_is_word_char(code))
        word_buf[word_buf_size++] = (struct word_buf_item){stream_x, code};
    else
    {
        NYX_VECTOR v = nyx_vector(stream_left+stream_x, stream_top, stream_y);

        if(_pipeline_mask(v, mask, stream_color))
            return -1;
    }

    stream_x += mask.w + stream_font->h_spacing;
    stream_prev = code;
    return 0;
}

static int stream_draw_string(bool nullterm, const char *str, size_t size) {
    size_t idx = 0;

    while(idx < size)
    {
        uint32_t code;

        if(_unicode_decode(str, size, &idx, &code))
            return -1;
        if(!code && nullterm)
            break;
        if(stream_draw_char(code))
            return -1;
    }
    word_buf_flush();
    return 0;
}

int _text_draw_string(const NYX_FONT *font, int32_t x, int32_t y, int wrap, bool multiline, bool nullterm, const char *str, size_t size, NYX_COLOR color) {
    reset_stream();
    stream_font = font;
    stream_color = color;
    stream_left = x;
    stream_top = y;
    stream_multiline = multiline;
    stream_wrap = wrap;
    stream_word_wrap = true;

    return stream_draw_string(nullterm, str, size);
}
