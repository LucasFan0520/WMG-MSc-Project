/* F006.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct item {
    char *name;
    int count;
    struct item *next;
} Item;

Item *head = NULL;

Item *find_item(const char *name) {
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void add_item(const char *name, int cnt) {
    Item *i = find_item(name);
    if (i) { i->count += cnt; return; }
    Item *new = malloc(sizeof(Item));
    new->name = strdup(name);
    new->count = cnt;
    new->next = head;
    head = new;
}

void take_item(const char *name, int cnt) {
    Item *i = find_item(name);
    if (!i) return;
    i->count -= cnt;
    if (i->count < 0) i->count = 0;
}

void remove_item(const char *name) {
    Item *i = head, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
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

void report() {
    Item *i = head;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256];
        int cnt;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report(); continue; }
        if (sscanf(line, "%*s %255s %d", name, &cnt) == 2) {
            if (strcmp(cmd, "ADD") == 0) add_item(name, cnt);
            else if (strcmp(cmd, "TAKE") == 0) take_item(name, cnt);
            else if (strcmp(cmd, "COUNT") == 0) count_item(name);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) remove_item(name);
        }
    }
    return 0;
}