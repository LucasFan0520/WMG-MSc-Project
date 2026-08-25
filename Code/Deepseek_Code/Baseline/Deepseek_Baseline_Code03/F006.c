// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct item {
    char *name;
    int count;
    struct item *next;
};

struct item *items = NULL;
struct item *tail = NULL;

struct item *find_item(char *name) {
    struct item *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void add_item(char *name, int count) {
    struct item *i = find_item(name);
    if (i) {
        i->count += count;
    } else {
        i = malloc(sizeof(struct item));
        i->name = malloc(strlen(name) + 1);
        strcpy(i->name, name);
        i->count = count;
        i->next = NULL;
        if (tail) {
            tail->next = i;
            tail = i;
        } else {
            items = tail = i;
        }
    }
}

void take_item(char *name, int count) {
    struct item *i = find_item(name);
    if (i) {
        i->count -= count;
        if (i->count < 0) i->count = 0;
    }
}

void remove_item(char *name) {
    struct item *i = items, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
            if (tail == i) tail = prev;
            free(i->name);
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void count_item(char *name) {
    struct item *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void report_items() {
    struct item *i = items;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256];
        int count;
        int n = sscanf(line, "%31s %255s %d", cmd, name, &count);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0 && n >= 3) {
            take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0 && n >= 2) {
            count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    return 0;
}