// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *items = NULL;

Item *find_item(const char *name) {
    Item *cur = items;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_item(const char *name, int count) {
    Item *it = find_item(name);
    if (it) {
        it->count += count;
    } else {
        it = malloc(sizeof(Item));
        if (!it) return;
        it->name = safe_strdup(name);
        it->count = count;
        it->next = items;
        items = it;
    }
}

void take_item(const char *name, int count) {
    Item *it = find_item(name);
    if (it) {
        it->count -= count;
        if (it->count < 0) it->count = 0;
    }
}

void remove_item(const char *name) {
    Item *prev = NULL, *cur = items;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else items = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void count_item(const char *name) {
    Item *it = find_item(name);
    printf("%d\n", it ? it->count : 0);
}

void report_items(void) {
    Item *cur = items;
    while (cur) {
        printf("%s %d\n", cur->name, cur->count);
        cur = cur->next;
    }
}

void free_all(void) {
    Item *cur = items;
    while (cur) {
        Item *next = cur->next;
        free(cur->name);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        char arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            int count = atoi(arg2);
            add_item(arg1, count);
        } else if (strcmp(cmd, "TAKE") == 0 && n >= 3) {
            int count = atoi(arg2);
            take_item(arg1, count);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_item(arg1);
        } else if (strcmp(cmd, "COUNT") == 0 && n >= 2) {
            count_item(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}