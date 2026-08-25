/* F006.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct item {
    char *name;
    int count;
    struct item *next;
} item_t;

item_t *head = NULL;
item_t *tail = NULL;

item_t *find_item(const char *name) {
    item_t *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void add_item(const char *name, int cnt) {
    item_t *i = find_item(name);
    if (i) {
        i->count += cnt;
        return;
    }
    i = malloc(sizeof(item_t));
    if (!i) return;
    i->name = strdup(name);
    i->count = cnt;
    i->next = NULL;
    if (!i->name) { free(i); return; }
    if (tail) tail->next = i;
    else head = i;
    tail = i;
}

void take_item(const char *name, int cnt) {
    item_t *i = find_item(name);
    if (!i) return;
    i->count -= cnt;
    if (i->count < 0) i->count = 0;
}

void remove_item(const char *name) {
    item_t *prev = NULL, *i = head;
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
    item_t *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void report_items(void) {
    item_t *i = head;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

void free_all(void) {
    item_t *i = head;
    while (i) {
        item_t *nxt = i->next;
        free(i->name);
        free(i);
        i = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], name[256];
        int cnt;
        if (sscanf(line, "%9s %255s %d", cmd, name, &cnt) >= 2) {
            if (strcmp(cmd, "ADD") == 0) {
                add_item(name, cnt);
            } else if (strcmp(cmd, "TAKE") == 0) {
                take_item(name, cnt);
            } else if (strcmp(cmd, "REMOVE") == 0) {
                remove_item(name);
            } else if (strcmp(cmd, "COUNT") == 0) {
                count_item(name);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_items();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}