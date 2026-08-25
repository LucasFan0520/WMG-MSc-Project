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

Item *find_item(const char *name) {
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
        return;
    }
    Item *new_i = malloc(sizeof(Item));
    if (!new_i) return;
    new_i->name = strdup(name);
    new_i->count = count;
    new_i->next = NULL;
    if (tail) tail->next = new_i;
    else head = new_i;
    tail = new_i;
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    i->count -= count;
    if (i->count < 0) i->count = 0;
}

void remove_item(const char *name) {
    Item *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_item(const char *name) {
    Item *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void report_items(void) {
    Item *i = head;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

void free_all(void) {
    Item *i = head;
    while (i) {
        Item *tmp = i;
        i = i->next;
        free(tmp->name);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256];
        int count;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line + 4, "%255s %d", name, &count) == 2) {
                add_item(name, count);
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            if (sscanf(line + 5, "%255s %d", name, &count) == 2) {
                take_item(name, count);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_item(p);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            count_item(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}