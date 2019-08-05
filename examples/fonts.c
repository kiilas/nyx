#include "nyx/nyx.h"

#include <stdio.h>

int font_id;

int main(void) {
    if(nyx_init())
        return -1;
    font_id = nyx_import_font("/home/kiilas/prog/nyx/fonts/basic.png");
    printf("nyx_import_font returned: %d\n", font_id);
    return 0;
}
