#ifndef NYX_GRAPHICS_H
#define NYX_GRAPHICS_H

int nyx_width(void);
int nyx_height(void);
int nyx_set_resolution(int width, int height);

int nyx_num_ramps(void);
int nyx_num_values(void);

float nyx_dither_squish(void);
void nyx_set_dither_squish(float squish);

NYX_COLOR nyx_get_background_color(void);
void nyx_set_background_color(NYX_COLOR color);

void nyx_screen_overlap(int x1, int y1, int x2, int y2, int *nx1, int *ny1, int *nx2, int *ny2);

#endif
