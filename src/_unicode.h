#ifndef NYX__UNICODE_H
#define NYX__UNICODE_H

int _unicode_decode(const char *str, size_t n, size_t *idx, uint32_t *code);

bool _unicode_is_whitespace(uint32_t code);
bool _unicode_is_word_char(uint32_t code);

#endif
