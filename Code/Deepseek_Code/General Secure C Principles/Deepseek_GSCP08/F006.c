// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item { char *name; int count; struct Item *next; } Item;
Item *items = NULL, *tail = NULL;

Item *find_item(const char *name) {
    for (Item *i = items; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

void add_item(const char *name, int c) {
    Item *i = find_item(name);
    if (i) { i->count += c; return; }
    i = malloc(sizeof(Item));
    if (!i) return;
    i->name = strdup(name); i->count = c; i->next = NULL;
    if (tail) tail->next = i; else items = i;
    tail = i;
}

void take_item(const char *name, int c) {
    Item *i = find_item(name);
    if (!i) return;
    if (i->count > c) i->count -= c;
    else i->count = 0;
}

void remove_item(const char *name) {
    Item *prev = NULL;
    for (Item *i = items; i; prev = i, i = i->next) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next; else items = i->next;
            if (i == tail) tail = prev;
            free(i->name); free(i);
            return;
        }
    }
}

void count_item(const char *name) {
    Item *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void report_items(void) {
    for (Item *i = items; i; i = i->next) printf("%s %d\n", i->name, i->count);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256]; int c;
            if (sscanf(rest, "%255s %d", name, &c) == 2) add_item(name, c);
        } else if (strcmp(cmd, "TAKE") == 0) {
            char name[256]; int c;
            if (sscanf(rest, "%255s %d", name, &c) == 2) take_item(name, c);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    while (items) { Item *i = items; items = i->next; free(i->name); free(i); }
    return 0;
}