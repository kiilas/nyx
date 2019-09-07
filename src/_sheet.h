#ifndef NYX__SHEET_H
#define NYX__SHEET_H

int _sheet_next(const NYX_BITMAP *, uint32_t border_color, int *x, int *y);
void _sheet_mask_bits_ptr(const NYX_BITMAP *bitmap, int x, int y, int w, int h, void *bits);
void *_sheet_mask_bits(const NYX_BITMAP *bitmap, int x, int y, int w, int h);
void _sheet_size(const NYX_BITMAP *bitmap, uint32_t border_color, int x, int y, int *w, int *h);
NYX_MASK *_sheet_mask(const NYX_BITMAP *bitmap, uint32_t border_color, int *x, int *y);

#endif
