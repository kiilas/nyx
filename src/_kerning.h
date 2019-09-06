#ifndef NYX__KERNING_H
#define NYX__KERNING_H

NYX_MAP *_kerningmap_init(void);

int _load_kernings(NYX_MAP *kernings, NYX_FILE *file);
int _save_kernings(const NYX_MAP *kernings, NYX_FILE *file);

#endif
