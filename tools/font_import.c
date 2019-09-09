#include "nyx/nyx.h"

int main(int argc, char **argv) {
    int font_id;

    if(argc != 3)
    {
        printf("font_import IN OUT\n");
        return 2;
    }
    if(nyx_init())
        return -1;
    font_id = nyx_font_import_ascii(argv[1]);
    if(font_id < 0)
        return -1;
    nyx_select_font(font_id);
    nyx_set_replacement_glyph('?');
    if(nyx_font_save(argv[2]))
        return -1;
    return 0;
}
