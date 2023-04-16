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
    return q->front == (void*)0;
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
