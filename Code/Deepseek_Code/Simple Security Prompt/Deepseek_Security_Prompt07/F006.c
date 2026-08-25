// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void add_item(const char *name, int cnt) {
    Item *i = find_item(name);
    if (i) {
        i->count += cnt;
    } else {
        i = malloc(sizeof(Item));
        i->name = strdup(name);
        i->count = cnt;
        i->next = items;
        items = i;
    }
}

void take_item(const char *name, int cnt) {
    Item *i = find_item(name);
    if (!i) return;
    if (i->count < cnt) i->count = 0;
    else i->count -= cnt;
}

void remove_item(const char *name) {
    Item *prev = NULL;
    Item *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
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
    Item *i = items;
    while (i) {
        printf("%s:%d\n", i->name, i->count);
        i = i->next;
    }
}

void free_all() {
    Item *i = items;
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int cnt;
            if (sscanf(rest, "%255s %d", name, &cnt) == 2) {
                add_item(name, cnt);
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char name[256];
            int cnt;
            if (sscanf(rest, "%255s %d", name, &cnt) == 2) {
                take_item(name, cnt);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                remove_item(name);
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                count_item(name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}