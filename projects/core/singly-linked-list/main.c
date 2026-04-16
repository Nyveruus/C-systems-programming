/*
singly linked list. Create and print the number of 10 nodes, then delete all of them
*/
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int number;
    struct node *next;
} node;

void spawn_node(int data, node **head);

int main(int argc, char *argv[]) {
    node *head = NULL;

    for (int i = 0; i < 10; i++) {
        spawn_node(i, &head);
    }

    node *cursor = head;
    while (cursor != NULL) {
        printf("%i\n", cursor->number);
        cursor = cursor->next;
    }

    while (head != NULL) {
        node *temp = head;
        head = head->next;
        free(temp);
    }
}

void spawn_node(int data, node **head) {
    node *new_node = malloc(sizeof(node));
    new_node->number = data;
    new_node->next = *head;
    *head = new_node;
}
