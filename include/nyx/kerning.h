#ifndef NYX_KERNING_H
#define NYX_KERNING_H

typedef struct NYX_KERNING_PAIR NYX_KERNING_PAIR;

int nyx_font_kerning(void);
int nyx_font_set_kerning(int kerning);
int nyx_font_kerning_num_pairs(size_t *pairs);
int nyx_font_kerning_pair(uint32_t prev, uint32_t next);
int nyx_font_kerning_pair_set(uint32_t prev, uint32_t next, int16_t offset);
int nyx_font_kerning_pair_by_index(uint64_t index, NYX_KERNING_PAIR *pair);

int nyx_font_kerning_auto_pair(uint32_t prev, uint32_t next);
int nyx_font_kerning_auto_range(uint32_t prev_min, uint32_t prev_max, uint32_t next_min, uint32_t next_max);
int nyx_font_kerning_auto_ranges(int num_ranges, const uint32_t *range_pairs);

#endif
