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

static inline void merge_lists(struct list_head *l1,
                               struct list_head *l2,
                               bool descend)
{
    struct list_head *ptr = l1;
    struct list_head *right = l1->next;
    struct list_head *left = l2->next;
    while (right != l1 && left != l2) {
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
    if (right == l1) {
        ptr->next = left;
        left->prev = ptr;
        l2->prev->next = l1;
        l1->prev = l2->prev;
    } else {
        ptr->next = right;
        right->prev = ptr;
    }
    l2->next = l2;
    l2->prev = l2;
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
    if (head == NULL || list_empty(head))
        return false;
    if (list_is_singular(head))
        return true;

    struct list_head *prev = NULL;
    struct list_head *curr = head->next;
    while (!(curr == head || curr->next == head)) {
        struct list_head *next = curr->next;
        element_t *curr_elem = list_entry(curr, element_t, list);
        element_t *next_elem = list_entry(next, element_t, list);
        if (strcmp(curr_elem->value, next_elem->value) == 0) {
            while (next != head &&
                   strcmp(curr_elem->value, next_elem->value) == 0) {
                struct list_head *tmp = next->next;
                list_del(next);
                q_release_element(next_elem);
                next = tmp;
                if (next != head)
                    next_elem = list_entry(next, element_t, list);
            }
            list_del(curr);
            q_release_element(curr_elem);
            if (prev) {
                prev->next = next;
                next->prev = prev;
                curr = next;
            } else {
                if (next) {
                    head->next = next;
                    next->prev = head;
                }
                curr = next;
            }
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *curr = head->next;
    while (curr != head && curr->next != head) {
        // swap
        struct list_head *tmp = curr->next;
        curr->prev->next = tmp;
        tmp->prev = curr->prev;
        curr->next = tmp->next;
        tmp->next->prev = curr;
        tmp->next = curr;
        curr->prev = tmp;
        curr = curr->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *curr = head;
    do {
        struct list_head *tmp = curr->next;
        curr->next = curr->prev;
        curr->prev = tmp;
        curr = tmp;
    } while (curr != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k <= 1)
        return;

    struct list_head *curr = head->next;
    struct list_head *group_prev = head;
    while (curr != head) {
        // check if there are k nodes left
        struct list_head *check = curr;
        int count = 0;
        while (check != head && count < k) {
            check = check->next;
            count++;
        }
        if (count < k) {
            break;
        }
        // reverse the group
        while (curr != check) {
            struct list_head *tmp = curr->next;
            curr->next = curr->prev;
            curr->prev = tmp;
            curr = tmp;
        }
        struct list_head *tmp = group_prev->next;
        group_prev->next = check->prev;
        check->prev->prev = group_prev;
        tmp->next = check;
        check->prev = tmp;
        group_prev = tmp;
        curr = check;
    }
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
    merge_lists(head, &left_head, descend);
}

static inline int order(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    int total_size = q_size(head);
    struct list_head *curr = descend ? head->prev : head->next;
    const char *max_val = NULL;
    while (curr != head) {
        element_t *curr_elem = list_entry(curr, element_t, list);
        struct list_head *next = descend ? curr->prev : curr->next;
        if (max_val == NULL || strcmp(curr_elem->value, max_val) >= 0) {
            max_val = curr_elem->value;
        } else {
            // remove curr
            list_del(curr);
            q_release_element(curr_elem);
            total_size--;
        }
        curr = next;
    }
    return total_size;
}


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return order(head, false);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return order(head, true);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head)) {
        return q_size(list_entry(head->next, queue_contex_t, chain)->q);
    }

    // separate left and right halves
    struct list_head *mid = find_mid(head);
    struct list_head left_head;
    INIT_LIST_HEAD(&left_head);
    list_cut_position(&left_head, head, mid->prev);

    // sort left and right halves
    int left_sz = q_merge(&left_head, descend);
    int right_sz = q_merge(head, descend);

    // merge two sorted halves
    queue_contex_t *q1 = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *q2 = list_entry(left_head.next, queue_contex_t, chain);
    merge_lists(q1->q, q2->q, descend);

    // update size
    q1->size = left_sz + right_sz;
    q2->size = 0;

    // merge chain nodes
    list_splice_tail(&left_head, head);
    return left_sz + right_sz;
}
