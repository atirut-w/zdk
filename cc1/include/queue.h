#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueNode {
  void *data;
  struct QueueNode *next;
} QueueNode;

/**
 * A generic FIFO queue.
 */
typedef struct {
  QueueNode *front;
  QueueNode *rear;
} Queue;

/**
 * Allocates and initializes a new queue.
 */
Queue *queue_new();

/**
 * Frees a queue and its nodes. Does not free the data pointers.
 */
void queue_free(Queue *queue);

/**
 * Enqueues an element to the rear of the queue.
 */
void queue_enqueue(Queue *queue, void *data);

/**
 * Dequeues an element from the front of the queue. Returns NULL if the queue
 * is empty.
 */
void *queue_dequeue(Queue *queue);

#endif /* QUEUE_H */
