/* F006.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *head = NULL;
Item *tail = NULL;

Item* find_item(const char *name) {
    Item *i = head;
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
        i = malloc(sizeof(Item));
        i->name = strdup(name);
        i->count = count;
        i->next = NULL;
        if (tail) tail->next = i;
        else head = i;
        tail = i;
    }
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    if (i->count < count) i->count = 0;
    else i->count -= count;
}

void remove_item(const char *name) {
    Item *i = head, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            if (i == tail) tail = prev;
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

void report_items() {
    Item *i = head;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

void free_all() {
    while (head) {
        Item *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char name[256]; int count;
        int n = sscanf(line, "%31s %255s %d", cmd, name, &count);
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0 && n == 3) {
            take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 2) {
            remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0 && n == 2) {
            count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_items();
        }
    }
    free_all();
    return 0;
}