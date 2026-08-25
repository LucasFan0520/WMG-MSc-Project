/* F006.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *items = NULL;

Item *find_item(const char *name) {
    Item *i = items;
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
    } else {
        Item *ni = malloc(sizeof(Item));
        ni->name = strdup(name);
        ni->count = count;
        ni->next = items;
        items = ni;
    }
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (i) {
        i->count -= count;
        if (i->count < 0) i->count = 0;
    }
}

void remove_item(const char *name) {
    Item *prev = NULL, *cur = items;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else items = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void free_items(void) {
    while (items) {
        Item *tmp = items;
        items = items->next;
        free(tmp->name);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256];
        int count;
        int n = sscanf(line, "%s %s %d", cmd, name, &count);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 3) continue;
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0) {
            if (n < 3) continue;
            take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (n < 2) continue;
            remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (n < 2) continue;
            Item *i = find_item(name);
            printf("%d\n", i ? i->count : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            Item *i = items;
            while (i) {
                printf("%s %d\n", i->name, i->count);
                i = i->next;
            }
        }
    }
    free_items();
    return 0;
}