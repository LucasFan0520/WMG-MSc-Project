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
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void free_item(Item *i) {
    free(i->name);
    free(i);
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
            Item *last = head;
            while (last->next) last = last->next;
            last->next = new;
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
    Item *i = head, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            free_item(i);
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *cntstr = strtok(NULL, " ");
            if (name && cntstr) {
                int cnt = atoi(cntstr);
                add_item(name, cnt);
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *name = strtok(NULL, " ");
            char *cntstr = strtok(NULL, " ");
            if (name && cntstr) {
                int cnt = atoi(cntstr);
                take_item(name, cnt);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    while (head) {
        Item *tmp = head;
        head = head->next;
        free_item(tmp);
    }
    return 0;
}