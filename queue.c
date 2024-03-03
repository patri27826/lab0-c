#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *it, *safe;
    list_for_each_entry_safe (it, safe, head, list) {
        q_release_element(it);
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *head_element = list_first_entry(head, element_t, list);
    list_del(&head_element->list);

    if (sp && bufsize) {
        memcpy(sp, head_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    return head_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *last_element = list_last_entry(head, element_t, list);
    list_del(&last_element->list);
    if (sp) {
        memcpy(sp, last_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    return last_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next, *fast = head->next, *tail = head->prev;

    while (fast != tail && fast->next != tail) {
        slow = slow->next;
        fast = fast->next->next;
    }

    list_del(slow);
    element_t *e = list_entry(slow, element_t, list);
    q_release_element(e);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;

    bool isDup = false;
    element_t *node, *safe;
    list_for_each_entry_safe (node, safe, head, list) {
        if (&safe->list != head && !strcmp(safe->value, node->value)) {
            list_del(&node->list);
            q_release_element(node);
            isDup = true;
        } else if (isDup) {
            list_del(&node->list);
            q_release_element(node);
            isDup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || !head->next || list_empty(head))
        return;

    struct list_head *cur = head->next, *next = head->next->next;
    element_t *e1, *e2;

    while (cur != head && next != head) {
        e1 = list_entry(cur, element_t, list);
        e2 = list_entry(next, element_t, list);

        char *c = e1->value;
        e1->value = e2->value;
        e2->value = c;

        cur = next->next;
        if (!next->next) {
            break;
        }
        next = next->next->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe;

    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head)
        return;

    struct list_head *last = head->next;
    int times = q_size(head) / k;
    LIST_HEAD(tmp);
    LIST_HEAD(result);

    for (int i = 0; i < times; i++) {
        for (int j = 0; j < k; j++) {
            struct list_head *node = last->next;
            list_del(last);
            list_add(last, &tmp);
            last = node;
        }
        list_splice_tail_init(&tmp, &result);
    }
    list_splice_init(&result, head);
}

struct list_head *_merge_sorted_single_linked_queues(struct list_head *l1,
                                                     struct list_head *l2)
{
    struct list_head *head = NULL, **ptr = &head;
    for (struct list_head **cur = NULL; l1 && l2; *cur = (*cur)->next) {
        if (strcmp(container_of(l1, element_t, list)->value,
                   container_of(l2, element_t, list)->value) >= 0)
            cur = &l2;
        else
            cur = &l1;
        *ptr = *cur;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) (void *) ((uintptr_t) (void *) l1 |
                                          (uintptr_t) (void *) l2);
    return head;
}

struct list_head *_merge_sort(struct list_head *l)
{
    if (l == NULL || l->next == NULL)
        return l;
    struct list_head *fast = l;
    struct list_head *slow = l;
    while (fast->next != NULL && fast->next->next != NULL) {
        fast = fast->next->next;
        slow = slow->next;
    }
    struct list_head *l1 = l;
    struct list_head *l2 = slow->next;
    slow->next = NULL;

    return _merge_sorted_single_linked_queues(_merge_sort(l1), _merge_sort(l2));
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (head == NULL || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = _merge_sort(head->next);

    struct list_head *cur = head;
    struct list_head *next = head->next;
    while (next) {
        next->prev = cur;
        cur = next;
        next = next->next;
    }
    cur->next = head;
    head->prev = cur;

    if (descend) {
        q_reverse(head);
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *first = list_entry(head->prev, element_t, list);
    element_t *second = list_entry(head->prev->prev, element_t, list);

    while (&second->list != head) {
        if (strcmp(first->value, second->value) > 0) {
            first = list_entry(first->list.prev, element_t, list);
            second = list_entry(second->list.prev, element_t, list);
        } else {
            list_del(&second->list);
            q_release_element(second);
            second = list_entry(first->list.prev, element_t, list);
        }
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *first = list_entry(head->prev, element_t, list);
    element_t *second = list_entry(head->prev->prev, element_t, list);

    while (&second->list != head) {
        if (strcmp(first->value, second->value) < 0) {
            first = list_entry(first->list.prev, element_t, list);
            second = list_entry(second->list.prev, element_t, list);
        } else {
            list_del(&second->list);
            q_release_element(second);
            second = list_entry(first->list.prev, element_t, list);
        }
    }

    return q_size(head);
}
int _merge_sorted_double_linked_queues(struct list_head *l1,
                                       struct list_head *l2)
{
    if (!l1 || !l2)
        return 0;

    LIST_HEAD(head);
    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *ele_1 = list_first_entry(l1, element_t, list);
        element_t *ele_2 = list_first_entry(l2, element_t, list);
        element_t *ele_min =
            strcmp(ele_1->value, ele_2->value) < 0 ? ele_1 : ele_2;
        list_move_tail(&ele_min->list, &head);
    }

    list_splice_tail_init(l1, &head);
    list_splice_tail_init(l2, &head);
    list_splice(&head, l1);

    return q_size(l1);
}
/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return q_size(list_entry(head, queue_contex_t, chain)->q);

    int size = q_size(head);
    int count = (size % 2) ? size / 2 + 1 : size / 2;
    int queue_size = 0;
    for (int i = 0; i < count; ++i) {
        queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
        queue_contex_t *second =
            list_entry(first->chain.next, queue_contex_t, chain);
        while (!list_empty(first->q) && !list_empty(second->q)) {
            queue_size =
                _merge_sorted_double_linked_queues(first->q, second->q);
            list_move_tail(&second->chain, head);
            first = list_entry(first->chain.next, queue_contex_t, chain);
            second = list_entry(first->chain.next, queue_contex_t, chain);
        }
    }

    if (descend) {
        q_reverse(head);
    }
    return queue_size;
}
