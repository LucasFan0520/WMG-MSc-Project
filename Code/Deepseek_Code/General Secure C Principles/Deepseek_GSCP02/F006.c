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
    } else {
        i = malloc(sizeof(Item));
        if (!i) return;
        i->name = strdup(name);
        i->count = count;
        i->next = NULL;
        if (tail) {
            tail->next = i;
            tail = i;
        } else {
            head = tail = i;
        }
    }
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    if (i->count > count) i->count -= count;
    else i->count = 0;
}

void remove_item(const char *name) {
    Item *prev = NULL;
    Item *i = head;
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

void report_items(void) {
    Item *i = head;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

void free_all(void) {
    while (head) {
        Item *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
    tail = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int count;
            if (sscanf(line, "%*s %255s %d", name, &count) == 2) {
                add_item(name, count);
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char name[256];
            int count;
            if (sscanf(line, "%*s %255s %d", name, &count) == 2) {
                take_item(name, count);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                remove_item(name);
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                count_item(name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }

    free(line);
    free_all();
    return 0;
}