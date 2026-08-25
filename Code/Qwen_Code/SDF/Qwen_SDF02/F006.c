// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *head = NULL, *tail = NULL;

Item* find_item(const char *name) {
    for (Item *i = head; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int val;
            if (sscanf(rest, "%255s %d", name, &val) != 2) continue;
            Item *i = find_item(name);
            if (i) i->count += val;
            else {
                i = malloc(sizeof(Item));
                if (!i) continue;
                i->name = strdup(name);
                i->count = val;
                i->next = NULL;
                if (!head) head = tail = i;
                else { tail->next = i; tail = i; }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char name[256];
            int val;
            if (sscanf(rest, "%255s %d", name, &val) != 2) continue;
            Item *i = find_item(name);
            if (i) {
                i->count -= val;
                if (i->count < 0) i->count = 0;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Item *prev = NULL;
            for (Item *i = head; i; prev = i, i = i->next) {
                if (strcmp(i->name, name) == 0) {
                    if (prev) prev->next = i->next;
                    else head = i->next;
                    if (tail == i) tail = prev;
                    free(i->name);
                    free(i);
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Item *i = find_item(name);
            printf("%d\n", i ? i->count : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item *i = head; i; i = i->next)
                printf("%s %d\n", i->name, i->count);
        }
    }
    while (head) {
        Item *i = head;
        head = head->next;
        free(i->name);
        free(i);
    }
    return 0;
}