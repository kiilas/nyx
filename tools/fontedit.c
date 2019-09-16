#include "nyx/nyx.h"

#define NONE          0
#define OUTPUT        1
#define IMPORT        2
#define GLYPH         3
#define GLYPH_RANGE   4
#define REPLACEMENT   5

#define SPACING      10
#define H_SPACING    11
#define V_SPACING    12
#define SPACING_AUTO 13

#define KERNING_AUTO 20

#define EXPR_ARGS 2

static int font_id;

static const char *out_path;
static NYX_BITMAP *import_bmp;
static int import_x;
static int import_y;

static NYX_LIST *auto_kerning_ranges;

static int mode;
static int cmd;
static int num_args;
static const char *cmd_args[EXPR_ARGS];

static int to_int(const char *str) {
    int i;

    if(sscanf(str, "%d", &i) != 1)
    {
        printf("Error reading integer value: %s\n", str);
        exit(2);
    }
    return i;
}

static uint32_t to_code(const char *str) {
    unsigned long code;

    if(sscanf(str, "%lx", &code) != 1)
    {
        printf("Error reading hex codepoint: %s\n", str);
        exit(2);
    }
    return code;
}

static void import(const char *path) {
    if(import_bmp)
        nyx_destroy_bitmap(import_bmp);
    import_bmp = nyx_load_bitmap(path);
    if(!import_bmp)
    {
        printf("Error loading bitmap: %s\n", path);
        exit(-1);
    }
    import_x = 0;
    import_y = 0;
    mode = IMPORT;
}

static void glyphs_import(uint32_t from, uint32_t to) {
    if(nyx_font_glyphs_import_from_bitmap_xy(import_bmp,
                                             from,
                                             to,
                                             &import_x,
                                             &import_y))
    {
        printf("Error importing glyphs: %x to %x\n", from, to);
        exit(-1);
    }
}

static void auto_kerning_add(uint32_t from, uint32_t to) {
    uint32_t list_min;
    uint32_t list_max;
    size_t i;

    for(i=0; i<nyx_list_size(auto_kerning_ranges)/2; ++i)
    {
        list_min = *(uint32_t *)nyx_list_get_unsafe(auto_kerning_ranges, 2*i);
        list_max = *(uint32_t *)nyx_list_get_unsafe(auto_kerning_ranges, 2*i + 1);

        if(nyx_font_kerning_auto_range(list_min, list_max, from, to) ||
           nyx_font_kerning_auto_range(from, to, list_min, list_max))
            goto fail;
    }
    if(nyx_font_kerning_auto_range(from, to, from, to))
        goto fail;
    if(nyx_list_add(auto_kerning_ranges, &from) ||
       nyx_list_add(auto_kerning_ranges, &to))
    {
        printf("Error adding to auto-kerning list: %x to %x\n", from, to);
        exit(-1);
    }
    return;
fail:
    printf("Error adding auto-kerning for range: %x to %x\n", from, to);
    exit(-1);
}

static void glyphs(const char *from_str, const char *to_str) {
    unsigned long from = to_code(from_str);
    unsigned long to = to_code(to_str);

    if(mode == IMPORT)
        glyphs_import(from, to);
    else if(mode == KERNING_AUTO)
        auto_kerning_add(from, to);
    else
    {
        printf("Error: glyph arguments without context\n");
        exit(2);
    }
}

static void replacement(const char *arg) {
    uint32_t code = to_code(arg);

    if(nyx_set_replacement_glyph(code))
    {
        printf("Error: could not set replacement glyph\n");
        exit(-1);
    }
}

static void spacing(const char *arg) {
    int val = to_int(arg);

    if(nyx_set_font_spacing(val))
        printf("Warning: could not set spacing\n");
}

static void h_spacing(const char *arg) {
    int val = to_int(arg);

    if(nyx_set_font_h_spacing(val))
        printf("Warning: could not set horizontal spacing\n");
}

static void v_spacing(const char *arg) {
    int val = to_int(arg);

    if(nyx_set_font_v_spacing(val))
        printf("Warning: could not set vertical spacing\n");
}

static void auto_spacing(void) {
    if(nyx_set_font_spacing_auto())
        printf("Warning: could not set automatic spacing\n");
}

static void auto_kerning(void) {
    if(nyx_font_set_kerning(1))
    {
        printf("Error: couldn't enable kerning\n");
        exit(-1);
    }
    if(auto_kerning_ranges)
        nyx_destroy_list(auto_kerning_ranges);
    auto_kerning_ranges = nyx_list_init(4);
    if(!auto_kerning_ranges)
    {
        printf("Error: couldn't init auto kerning list\n");
        exit(-1);
    }
    mode = KERNING_AUTO;
}

static int get_cmd(const char *arg) {
    if(!strcmp(arg, "-o"))
    {
        cmd = OUTPUT;
        num_args = 1;
    }
    else if(!strcmp(arg, "-i"))
    {
        cmd = IMPORT;
        num_args = 1;
    }
    else if(!strcmp(arg, "-G"))
    {
        cmd = GLYPH;
        num_args = 1;
    }
    else if(!strcmp(arg, "-g"))
    {
        cmd = GLYPH_RANGE;
        num_args = 2;
    }
    else if(!strcmp(arg, "-r"))
    {
        cmd = REPLACEMENT;
        num_args = 1;
    }
    else if(!strcmp(arg, "-s"))
    {
        cmd = SPACING;
        num_args = 1;
    }
    else if(!strcmp(arg, "-h"))
    {
        cmd = H_SPACING;
        num_args = 1;
    }
    else if(!strcmp(arg, "-v"))
    {
        cmd = V_SPACING;
        num_args = 1;
    }
    else if(!strcmp(arg, "-S"))
    {
        cmd = SPACING_AUTO;
        num_args = 0;
    }
    else if(!strcmp(arg, "-K"))
    {
        cmd = KERNING_AUTO;
        num_args = 0;
    }
    else
        return -1;
    return 0;
}

static void process_cmd(void) {
    if(cmd == OUTPUT)
        out_path = cmd_args[0];
    else if(cmd == IMPORT)
        import(cmd_args[0]);
    else if(cmd == GLYPH)
        glyphs(cmd_args[0], cmd_args[0]);
    else if(cmd == GLYPH_RANGE)
        glyphs(cmd_args[0], cmd_args[1]);
    else if(cmd == REPLACEMENT)
        replacement(cmd_args[0]);
    else if(cmd == SPACING)
        spacing(cmd_args[0]);
    else if(cmd == H_SPACING)
        h_spacing(cmd_args[0]);
    else if(cmd == V_SPACING)
        v_spacing(cmd_args[0]);
    else if(cmd == SPACING_AUTO)
        auto_spacing();
    else if(cmd == KERNING_AUTO)
        auto_kerning();
}

int main(int argc, const char **argv) {
    int i = 1;

    if(nyx_init())
    {
        printf("Error initialising nyx\n");
        return -1;
    }
    font_id = nyx_make_font();
    if(font_id < 0)
    {
        printf("Error initialising font\n");
        return -1;
    }
    nyx_select_font(font_id);
    while(i < argc)
    {
        int cmd_arg;

        if(get_cmd(argv[i]))
            return 2;
        if(i + 1 + num_args > argc)
            return 2;
        for(cmd_arg=0; cmd_arg<num_args; ++cmd_arg)
            cmd_args[cmd_arg] = argv[i + 1 + cmd_arg];
        i += 1 + num_args;
        process_cmd();
    }
    if(out_path && nyx_font_save(out_path))
    {
        printf("Error saving font\n");
        return -1;
    }
    printf("All ok!\n");
    return 0;
}
