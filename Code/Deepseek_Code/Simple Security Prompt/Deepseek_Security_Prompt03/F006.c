/*
 * F006.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct item {
    char *name;
    int count;
    struct item *next;
} Item;

Item *head = NULL, *tail = NULL;

Item *find_item(const char *name) {
    Item *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_item(const char *name, int cnt) {
    Item *p = find_item(name);
    if (p) {
        p->count += cnt;
    } else {
        p = malloc(sizeof(Item));
        p->name = strdup(name);
        p->count = cnt;
        p->next = NULL;
        if (tail) tail->next = p;
        else head = p;
        tail = p;
    }
}

void take_item(const char *name, int cnt) {
    Item *p = find_item(name);
    if (!p) return;
    p->count -= cnt;
    if (p->count < 0) p->count = 0;
}

void remove_item(const char *name) {
    Item *prev = NULL, *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (p == tail) tail = prev;
            free(p->name);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void count_item(const char *name) {
    Item *p = find_item(name);
    printf("%d\n", p ? p->count : 0);
}

void report_items(void) {
    Item *p = head;
    while (p) {
        printf("%s %d\n", p->name, p->count);
        p = p->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256];
        int cnt;
        if (sscanf(line, "%31s %255s %d", cmd, name, &cnt) >= 2) {
            if (strcmp(cmd, "ADD") == 0) {
                add_item(name, cnt);
            } else if (strcmp(cmd, "TAKE") == 0) {
                take_item(name, cnt);
            } else if (strcmp(cmd, "REMOVE") == 0) {
                remove_item(name);
            } else if (strcmp(cmd, "COUNT") == 0) {
                count_item(name);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_items();
            }
        }
    }
    return 0;
}