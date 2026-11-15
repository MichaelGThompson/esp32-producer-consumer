#include <stdint.h>
#include <stdbool.h>

#include "include/event_queue.h"

void Queue_Init(ButtonQueue_t *queue) {
    queue->head = 0;
    queue->tail = 0;
}

bool Queue_IsFull(ButtonQueue_t *queue) {
    return ((queue->head + 1) % QUEUE_SIZE) == queue->tail;
}

bool Queue_IsEmpty(ButtonQueue_t *queue) {
    return queue->head == queue->tail;
}

bool Queue_Enqueue(ButtonQueue_t *queue, uint8_t button_id) {  
    if (Queue_IsFull(queue)) {
        return false; // Queue overflow! 
    }
    
    queue->data[queue->head].event_id = button_id;
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    
    return true;
}

bool Queue_Dequeue(ButtonQueue_t *queue, ButtonEvent_t *event_out) {
    if (Queue_IsEmpty(queue)) {
        return false; // Nothing to process
    }
    
    *event_out = queue->data[queue->tail];
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    return true;
}
