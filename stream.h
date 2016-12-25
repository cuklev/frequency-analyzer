#ifndef __AMPLITUDE_H
#define __AMPLITUDE_H

#ifdef __cplusplus
extern "C" {
#endif

void open_stream(void);
void read_stream(void*, int);
void free_stream(void);

#ifdef __cplusplus
}
#endif

#endif
