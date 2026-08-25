// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct item {
    char *name;
    int count;
    struct item *next;
} Item;

Item *head = NULL;
Item *tail = NULL;

Item *find_item(const char *name) {
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void add_item(const char *name, int count) {
    Item *i = find_item(name);
    if (i) {
        i->count += count;
        return;
    }
    i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->count = count;
    i->next = NULL;
    if (tail) tail->next = i;
    else head = i;
    tail = i;
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    i->count -= count;
    if (i->count < 0) i->count = 0;
}

void remove_item(const char *name) {
    Item *i = head, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            if (i == tail) tail = prev;
            free(i->name);
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void count_item(const char *name) {
    Item *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void report_items(void) {
    Item *i = head;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256];
        int count;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &count) == 2)
                add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &count) == 2)
                take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    return 0;
}