// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item { char *name; int count; struct Item *next; } Item;
Item *head = NULL;

void add_item(const char *name, int c) {
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) { i->count += c; return; }
    }
    Item *i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->count = c;
    i->next = head;
    head = i;
}

void take_item(const char *name, int c) {
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) {
            i->count -= c;
            if (i->count < 0) i->count = 0;
            return;
        }
    }
}

void remove_item(const char *name) {
    Item **p = &head;
    while (*p) {
        if (strcmp((*p)->name, name) == 0) {
            Item *tmp = *p;
            *p = tmp->next;
            free(tmp->name);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void count_item(const char *name) {
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) { printf("%d\n", i->count); return; }
    }
    printf("0\n");
}

void report() {
    for (Item *i = head; i; i = i->next) printf("%s %d\n", i->name, i->count);
}

void cleanup() {
    while (head) {
        Item *tmp = head->next;
        free(head->name);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256];
        int c;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &c) == 2) add_item(name, c);
        } else if (strcmp(cmd, "TAKE") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &c) == 2) take_item(name, c);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) report();
    }
    cleanup();
    return 0;
}