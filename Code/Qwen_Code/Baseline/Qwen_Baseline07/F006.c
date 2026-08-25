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

int main() {
    char cmd[20], name[256];
    int c;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %d", name, &c);
            Item *it = head;
            while (it && strcmp(it->name, name) != 0) it = it->next;
            if (it) it->count += c;
            else {
                Item *ni = malloc(sizeof(Item));
                ni->name = strdup(name); ni->count = c; ni->next = NULL;
                if (!head) head = tail = ni; else { tail->next = ni; tail = ni; }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            scanf("%s %d", name, &c);
            for (Item *it = head; it; it = it->next)
                if (strcmp(it->name, name) == 0) { it->count -= c; if (it->count < 0) it->count = 0; }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", name);
            Item *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    if (!curr->next) tail = prev;
                    free(curr->name); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", name);
            int found = 0;
            for (Item *it = head; it; it = it->next)
                if (strcmp(it->name, name) == 0) { printf("%d\n", it->count); found = 1; break; }
            if (!found) printf("0\n");
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item *it = head; it; it = it->next) printf("%s %d\n", it->name, it->count);
        }
    }
    return 0;
}