#include "nyx/nyx.h"

#define GRID_W   16
#define GRID_H   16
#define GRID_D   16
#define STRIDE_X 256
#define STRIDE_Y 256
#define STRIDE_Z 256
#define RECT_W   16
#define RECT_H   16

#define OFFSET_X (((GRID_W-1) * STRIDE_X + RECT_W) / -2.0)
#define OFFSET_Y (((GRID_H-1) * STRIDE_Y + RECT_H) / -2.0)
#define OFFSET_Z STRIDE_Z

#define DIMMING 2048.0f

void tic(void) {
    if(nyx_key_pressed(NYX_KEY_RIGHT))
        nyx_view_pan3d(1, 0, 0);
    if(nyx_key_pressed(NYX_KEY_LEFT))
        nyx_view_pan3d(-1, 0, 0);
    if(nyx_key_pressed(NYX_KEY_UP))
        nyx_view_pan3d(0, -1, 0);
    if(nyx_key_pressed(NYX_KEY_DOWN))
        nyx_view_pan3d(0, 1, 0);
    if(nyx_key_pressed(NYX_KEY_Z))
        nyx_view_pan3d(0, 0, -1);
    if(nyx_key_pressed(NYX_KEY_X))
        nyx_view_pan3d(0, 0, 1);
}

int handle_event(NYX_EVENT *event) {
    if(event->type == NYX_EVENT_KEY_DOWN && event->keyboard.code == NYX_KEY_Q)
    {
        nyx_end();
        return 1;
    }
    return 0;
}

void render(void) {
    int32_t camera_z = -nyx_view_transform().simple_camera.trans.a[2];
    int rx, ry, rz;

    nyx_clear();
    for(rz=GRID_D-1; rz>=0; --rz)
    {
        int32_t z = rz * STRIDE_Z + OFFSET_Z;
        float brightness;

        if(z <= camera_z)
            continue;

        brightness = DIMMING / (DIMMING + z - camera_z);
        NYX_COLOR color = {(rz+1)%nyx_num_ramps(), 255 * brightness};
        for(rx=0; rx<GRID_W; ++rx)
            for(ry=0; ry<GRID_H; ++ry)
            {
                int32_t x = rx * STRIDE_X + OFFSET_X;
                int32_t y = ry * STRIDE_Y + OFFSET_Y;

                nyx_draw_rect3d(x, y, z, RECT_W, RECT_H, color);
            }
    }
}

int main(void) {
    NYX_TRANSFORM camera;

    if(nyx_init())
        return -1;
    nyx_set_tic_handler(tic);
    nyx_set_event_handler(handle_event);
    nyx_set_renderer(render);
    camera = nyx_trans_simple_camera(NYX_VECTOR_ZERO, 256);
    nyx_set_view_transform(camera);
    nyx_start();
    return 0;
}
