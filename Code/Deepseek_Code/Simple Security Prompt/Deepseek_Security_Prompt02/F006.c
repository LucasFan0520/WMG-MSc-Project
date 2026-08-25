/* F006.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *items = NULL;

Item *find_item(const char *name) {
    Item *i = items;
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
        return;
    }
    i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->count = count;
    i->next = items;
    items = i;
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    if (i->count < count) i->count = 0;
    else i->count -= count;
}

void remove_item(const char *name) {
    Item *prev = NULL, *curr = items;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else items = curr->next;
            free_item(curr);
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
    Item *i = items;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256];
    int count;

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d", name, &count) != 2) continue;
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d", name, &count) != 2) continue;
            take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }

    while (items) {
        Item *tmp = items;
        items = items->next;
        free_item(tmp);
    }
    return 0;
}