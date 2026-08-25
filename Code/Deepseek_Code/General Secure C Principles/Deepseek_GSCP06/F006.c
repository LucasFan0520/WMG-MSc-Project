// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *head = NULL;

Item *find_item(const char *name) {
    Item *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_item(const char *name, int count) {
    Item *i = find_item(name);
    if (i) {
        i->count += count;
    } else {
        i = malloc(sizeof(Item));
        i->name = strdup(name);
        i->count = count;
        i->next = NULL;
        if (!head) head = i;
        else {
            Item *cur = head;
            while (cur->next) cur = cur->next;
            cur->next = i;
        }
    }
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    i->count -= count;
    if (i->count < 0) i->count = 0;
}

void remove_item(const char *name) {
    Item *cur = head, *prev = NULL;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void count_item(const char *name) {
    Item *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void report_items(void) {
    Item *cur = head;
    while (cur) {
        printf("%s %d\n", cur->name, cur->count);
        cur = cur->next;
    }
}

void cleanup_items(void) {
    Item *cur = head;
    while (cur) {
        Item *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256];
        int count;
        int n = sscanf(line, "%31s %255s %d", cmd, name, &count);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0 && n == 3) {
            take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    cleanup_items();
    return 0;
}