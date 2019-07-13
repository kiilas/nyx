#ifndef NYX_META_H
#define NYX_META_H

int nyx_init(void);
int nyx_start(void);
void nyx_end(void);

uint64_t nyx_tic(void);

void nyx_set_tic_handler(void (*f)(void));
void nyx_set_event_handler(int (*f)(NYX_EVENT *));
void nyx_set_renderer(void (*f)(void));

#endif
