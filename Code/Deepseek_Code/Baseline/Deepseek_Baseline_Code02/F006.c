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
        return;
    }
    i = malloc(sizeof(Item));
    i->name = malloc(strlen(name) + 1);
    strcpy(i->name, name);
    i->count = count;
    i->next = NULL;
    if (tail) tail->next = i;
    else head = i;
    tail = i;
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    if (i->count >= count) i->count -= count;
    else i->count = 0;
}

void remove_item(const char *name) {
    Item *prev = NULL;
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            if (tail == i) tail = prev;
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
    Item *i = head;
    while (i) {
        Item *next = i->next;
        free(i->name);
        free(i);
        i = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            int count;
            if (sscanf(p, "%d", &count) != 1) continue;
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            int count;
            if (sscanf(p, "%d", &count) != 1) continue;
            take_item(name, count);
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