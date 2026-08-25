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

Item* find_item(const char *name) {
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
        Item *new = malloc(sizeof(Item));
        new->name = strdup(name);
        new->count = count;
        new->next = NULL;
        if (!head) head = new;
        else {
            Item *cur = head;
            while (cur->next) cur = cur->next;
            cur->next = new;
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
    Item *prev = NULL, *cur = head;
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

void free_all() {
    Item *cur = head;
    while (cur) {
        Item *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *cnt_str = strtok(NULL, " ");
            int cnt = cnt_str ? atoi(cnt_str) : 0;
            add_item(name, cnt);
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *cnt_str = strtok(NULL, " ");
            int cnt = cnt_str ? atoi(cnt_str) : 0;
            take_item(name, cnt);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            Item *i = find_item(name);
            printf("%d\n", i ? i->count : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            Item *cur = head;
            while (cur) {
                printf("%s %d\n", cur->name, cur->count);
                cur = cur->next;
            }
        }
    }
    free_all();
    return 0;
}