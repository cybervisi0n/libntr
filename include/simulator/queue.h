#ifndef SIMULATOR_QUEUE_H
#define SIMULATOR_QUEUE_H

typedef struct {
    size_t head;
    size_t tail;
    size_t size;
    void** data;
} SIM_queue_t;

#ifdef __cplusplus
extern "C" {
#endif

void* SIM_queue_read(SIM_queue_t *queue);

int SIM_queue_write(SIM_queue_t *queue, void* handle);

#ifdef __cplusplus
}
#endif

#endif