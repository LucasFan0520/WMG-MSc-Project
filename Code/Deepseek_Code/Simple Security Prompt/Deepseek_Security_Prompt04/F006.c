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

Item *find_item(char *name) {
    for (Item *i = head; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

void add_item(char *name, int count) {
    Item *i = find_item(name);
    if (i) i->count += count;
    else {
        Item *new = malloc(sizeof(Item));
        new->name = strdup(name);
        new->count = count;
        new->next = NULL;
        if (!tail) head = tail = new;
        else { tail->next = new; tail = new; }
    }
}

void take_item(char *name, int count) {
    Item *i = find_item(name);
    if (i) {
        i->count -= count;
        if (i->count < 0) i->count = 0;
    }
}

void remove_item(char *name) {
    Item *prev = NULL;
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            if (i == tail) tail = prev;
            free(i->name);
            free(i);
            return;
        }
        prev = i;
    }
}

void count_item(char *name) {
    Item *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void report_all(void) {
    for (Item *i = head; i; i = i->next)
        printf("%s %d\n", i->name, i->count);
}

void free_all(void) {
    Item *i = head;
    while (i) {
        Item *next = i->next;
        free(i->name);
        free(i);
        i = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256];
        int n = sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_item(arg1, atoi(arg2));
        } else if (strcmp(cmd, "TAKE") == 0 && n == 3) {
            take_item(arg1, atoi(arg2));
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 2) {
            remove_item(arg1);
        } else if (strcmp(cmd, "COUNT") == 0 && n == 2) {
            count_item(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}