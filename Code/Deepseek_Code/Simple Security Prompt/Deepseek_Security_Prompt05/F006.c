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
        return;
    }
    i = malloc(sizeof(Item));
    if (!i) return;
    i->name = strdup(name);
    i->count = count;
    i->next = items;
    items = i;
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    if (i->count > count) i->count -= count;
    else i->count = 0;
}

void remove_item(const char *name) {
    Item *i = items, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
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

void report_all(void) {
    Item *i = items;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256];
        int n = sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0) {
            int cnt = atoi(arg2);
            add_item(arg1, cnt);
        } else if (strcmp(cmd, "TAKE") == 0 && n == 3) {
            take_item(arg1, atoi(arg2));
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_item(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_item(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (items) {
        Item *tmp = items;
        items = items->next;
        free(tmp->name);
        free(tmp);
    }
    return 0;
}