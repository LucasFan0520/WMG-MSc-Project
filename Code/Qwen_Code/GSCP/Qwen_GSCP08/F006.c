// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *head = NULL;

Item *find_item(char *name) {
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) return i;
    }
    return NULL;
}

void add_item(char *name, int count) {
    Item *i = find_item(name);
    if (!i) {
        i = malloc(sizeof(Item));
        i->name = strdup(name);
        i->count = 0;
        i->next = head;
        head = i;
    }
    i->count += count;
}

void take_item(char *name, int count) {
    Item *i = find_item(name);
    if (i) {
        i->count -= count;
        if (i->count < 0) i->count = 0;
    }
}

void remove_item(char *name) {
    Item *prev = NULL;
    Item *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_item(char *name) {
    Item *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void report_items() {
    for (Item *i = head; i; i = i->next) {
        printf("%s %d\n", i->name, i->count);
    }
}

int main() {
    char cmd[20], name[100];
    int count;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s%d", name, &count);
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0) {
            scanf("%99s%d", name, &count);
            take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%99s", name);
            remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%99s", name);
            count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    return 0;
}