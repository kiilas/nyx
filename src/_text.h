#ifndef NYX__TEXT_H
#define NYX__TEXT_H

struct text_stream {
    int32_t  left;
    int32_t  top;
    int32_t  x;
    int32_t  y;
    uint32_t prev;
    int      wrap;
    bool     multiline;
};

struct text_stream _text_stream(int32_t left, int32_t top, int wrap, bool multiline);

int _text_draw_char(int32_t x, int32_t y, uint32_t code, const NYX_FONT *font, NYX_COLOR color);
int _text_stream_draw_char(struct text_stream *stream, const NYX_FONT *font, uint32_t code, NYX_COLOR color);

int _text_stream_draw_string(struct text_stream *stream, const NYX_FONT *font, bool nullterm, const char *str, size_t size, NYX_COLOR color);
int _text_draw_string(const NYX_FONT *font, int32_t x, int32_t y, int wrap, bool multiline, bool nullterm, const char *str, size_t size, NYX_COLOR color);

#endif
