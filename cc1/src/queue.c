#include "queue.h"
#include <stdlib.h>

Queue *queue_new() {
  Queue *queue = malloc(sizeof(Queue));
  if (!queue) {
    return NULL;
  }
  queue->front = NULL;
  queue->rear = NULL;
  return queue;
}

void queue_free(Queue *queue) {
  if (queue) {
    QueueNode *current = queue->front;
    while (current) {
      QueueNode *next = current->next;
      free(current);
      current = next;
    }
    free(queue);
  }
}

void queue_enqueue(Queue *queue, void *data) {
  QueueNode *new_node;

  if (!queue) {
    return;
  }
  new_node = malloc(sizeof(QueueNode));
  if (!new_node) {
    return;
  }

  new_node->data = data;
  new_node->next = NULL;

  if (queue->rear) {
    queue->rear->next = new_node;
  } else {
    queue->front = new_node;
  }

  queue->rear = new_node;
}

void *queue_dequeue(Queue *queue) {
  QueueNode *front_node;
  void *data;

  if (!queue || !queue->front) {
    return NULL;
  }

  front_node = queue->front;
  data = front_node->data;
  queue->front = front_node->next;

  if (!queue->front) {
    queue->rear = NULL;
  }

  free(front_node);
  return data;
}
