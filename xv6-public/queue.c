#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

/**
 * @brief Initialize the current queue.
 * @param q Process queue.
 */
void
init_queue(struct proc_queue* q, int tq) 
{
    q->time_quantum = tq;
    q->front = q->end = (void*)0;
    q->size = 0;
}

/**
 * @brief Return a boolean value whether current process queue is empty or not.
 * @param q Process queue.
 * @return Boolean value(0, 1) of emptyness.
 */
int
is_empty(struct proc_queue* q)
{
    return q->size == 0;
}

/**
 * @brief Return a first element of current queue.
 * @param q Process queue.
 * @return [struct proc*] First element of current queue.
 */
struct proc*
front(struct proc_queue* q) 
{
    return q->front;
}

/**
 * @brief Pop a first element of current queue.
 * @param q Process queue.
 */
void
pop_proc(struct proc_queue* q) 
{
    struct proc* front = q->front->next;
    q->front = front;
    q->size--;
}

/**
 * @brief Unlink a process from a queue.
 * @param q Process queue.
 * @param p A process to unlink.
 */
void
unlink_proc(struct proc_queue* q, struct proc* p)
{
    if (q->size == 0) panic("unlink_proc: queue is empty");
    if (q->size == 1) {
        q->front = q->end = (void*)0;
        q->size = 0;
        return; 
    }

    struct proc* prev = q->front;
    struct proc* curr = q->front->next;
    while (curr != p) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == q->front) {
        q->front = curr->next;
        q->end->next = q->front;
    } 
    else if (curr == q->end) {
        q->end = prev;
        q->end->next = q->front;
    } 
    else prev->next = curr->next;

    q->size--;
}

/**
 * @brief Push a process into a queue
 * @param q Process queue.
 * @param p A process to push.
 */
void
push_proc(struct proc_queue* q, struct proc* p)
{
    q->size++;
    p->run_ticks = 0;
    if (is_empty(q)) {
        q->front = q->end = p;
        p->next = p;
        return;
    }
    q->end->next = p;
    q->end = p;
    p->next = q->front;
}

/**
 * @brief Set a process as front of queue.
 * @param q Process queue.
 * @param p A process to set as front
 */
void
set_front(struct proc_queue* q, struct proc* p)
{
    q->front = p;
    q->end = p->next;
}

/**
 * @brief Check a process is already in a queue or not.
 * @param q Process queue.
 * @param p A process to check.
 * @return Boolean value(0, 1) of existence.
 */
int
exists(struct proc_queue* q, struct proc* p)
{
    if (is_empty(q)) return 0;
    struct proc* curr = q->front;
    do {
        if (curr == p) return 1;
        curr = curr->next;
    } 
    while (curr != q->front);
    return 0;
}

/**
 * @brief Clear a queue.
 * @param q Process queue.
 */
void
clear_queue(struct proc_queue* q)
{
    q->front = q->end = (void*)0;
    q->size = 0;
}

/******************************************************
 *  Priority Queue Implementation
 ******************************************************/

/**
 * @brief Heap init.
 * @param pq Process priority queue.
 */
void
init_pri_queue(struct proc_pri_queue* pq, int tq)
{
    pq->size = 0;
    pq->time_quantum = tq;
}

/**
 * @brief Heapify up.
 * @param pq Process priority queue.
 */
void
heapify_up(struct proc_pri_queue* pq)
{
    int i = pq->size;
    while (i > 1 && pq->node[i]->priority < pq->node[i / 2]->priority) {
        struct proc* tmp = pq->node[i];
        pq->node[i] = pq->node[i / 2];
        pq->node[i / 2] = tmp;
        i /= 2;
    }
}

/**
 * @brief Heapify down.
 * @param pq Process priority queue.
 */
void
heapify_down(struct proc_pri_queue* pq)
{
    int i = 1;
    while (i * 2 <= pq->size) {
        int j = i * 2;
        if (j + 1 <= pq->size && pq->node[j + 1]->priority < pq->node[j]->priority) j++;
        if (pq->node[i]->priority <= pq->node[j]->priority) break;
        struct proc* tmp = pq->node[i];
        pq->node[i] = pq->node[j];
        pq->node[j] = tmp;
        i = j;
    }
}

/**
 * @brief Push a process into a priority queue.
 * @param pq Process priority queue.
 * @param p A process to push.
 */
void
push_pri_proc(struct proc_pri_queue* pq, struct proc* p)
{
    if (++pq->size >= PQ_SIZE) panic("push_pri_proc: queue is full");
    pq->node[pq->size] = p;
    heapify_up(pq);
}

/**
 * @brief Pop a process from a priority queue.
 * @param pq Process priority queue.
 */
void
pop_pri_proc(struct proc_pri_queue* pq)
{
    if (pq->size == 0) panic("pop_pri_proc: queue is empty");
    pq->node[1] = pq->node[pq->size--];
    heapify_down(pq);
}

/**
 * @brief Return a first element of current priority queue.
 * @param pq Process priority queue.
 * @return [struct proc*] First element of current priority queue.
 */
struct proc*
top_pri_proc(struct proc_pri_queue* pq)
{
    if (pq->size == 0) panic("top_pri_proc: queue is empty");
    return pq->node[1];
}

/**
 * @brief Return a boolean value of priority queue is empty or not.
 * @param pq Process priority queue.
 * @return Boolean value(0, 1) of empty.
 */
int
is_pri_empty(struct proc_pri_queue* pq)
{
    return pq->size == 0;
}