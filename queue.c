#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

static inline struct list_head *find_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *slow = head->next;
    struct list_head *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_head = malloc(sizeof(struct list_head));
    if (!new_head)
        return NULL;
    INIT_LIST_HEAD(new_head);
    return new_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head)) {
        free(head);
        return;
    }
    struct list_head *curr = head->next;
    while (curr != head) {
        struct list_head *tmp = curr;
        curr = curr->next;
        // remove node from list
        list_del(tmp);
        // release resources of element
        element_t *elem = list_entry(tmp, element_t, list);
        q_release_element(elem);
    }
    // remove head
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;
    new_elem->value = strdup(s);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }
    list_add(&(new_elem->list), head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;
    new_elem->value = strdup(s);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }
    list_add_tail(&(new_elem->list), head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    // remove node(unlink) from head
    struct list_head *node = head->next;
    list_del(node);

    // copy value to sp
    element_t *elem = list_entry(node, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    // remove node(unlink) from head
    struct list_head *node = head->prev;
    list_del(node);

    // copy value to sp
    element_t *elem = list_entry(node, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each(li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head))
        return false;
    // find mid
    struct list_head *mid = find_mid(head);
    // remove mid
    list_del(mid);
    // release resources
    element_t *elem = list_entry(mid, element_t, list);
    q_release_element(elem);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    // separate left and right halves
    struct list_head *mid = find_mid(head);
    struct list_head left_head;
    INIT_LIST_HEAD(&left_head);
    list_cut_position(&left_head, head, mid->prev);

    // sort left and right halves
    q_sort(&left_head, descend);
    q_sort(head, descend);

    // merge two sorted halves
    struct list_head *ptr = head;
    struct list_head *right = head->next;
    struct list_head *left = left_head.next;
    while (right != head && left != &left_head) {
        const element_t *elem_right = list_entry(right, element_t, list);
        const element_t *elem_left = list_entry(left, element_t, list);
        if ((strcmp(elem_right->value, elem_left->value) < 0) ^ descend) {
            ptr->next = right;
            right->prev = ptr;
            right = right->next;
        } else {
            ptr->next = left;
            left->prev = ptr;
            left = left->next;
        }
        ptr = ptr->next;
    }
    if (right == head) {
        ptr->next = left;
        left->prev = ptr;
        left_head.prev->next = head;
        head->prev = left_head.prev;
    } else {
        ptr->next = right;
        right->prev = ptr;
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
