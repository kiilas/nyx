#ifndef NYX__PIPELINE_H
#define NYX__PIPELINE_H

void _update_pipeline(void);

int _pipeline_pixel(NYX_VECTOR v, NYX_COLOR color);
int _pipeline_rect(NYX_VECTOR v, int32_t w, int32_t h, NYX_COLOR color);
int _pipeline_texture(NYX_VECTOR v, int id);

#endif
