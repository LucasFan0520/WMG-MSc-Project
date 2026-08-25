// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *head = NULL, *tail = NULL;

void free_list() {
    while (head) {
        Item *t = head; head = head->next;
        free(t->name); free(t);
    }
}

Item *find_item(const char *name) {
    for (Item *i = head; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256];
        int c;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &c) != 2) continue;
            Item *i = find_item(name);
            if (i) i->count += c;
            else {
                i = malloc(sizeof(Item));
                i->name = strdup(name); i->count = c; i->next = NULL;
                if (tail) tail->next = i; else head = i;
                tail = i;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &c) != 2) continue;
            Item *i = find_item(name);
            if (i) {
                i->count -= c;
                if (i->count < 0) i->count = 0;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Item *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Item *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    if (curr == NULL) tail = prev;
                    free(tmp->name); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Item *i = find_item(name);
            printf("%d\n", i ? i->count : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item *i = head; i; i = i->next) printf("%s %d\n", i->name, i->count);
        }
    }
    free_list();
    return 0;
}