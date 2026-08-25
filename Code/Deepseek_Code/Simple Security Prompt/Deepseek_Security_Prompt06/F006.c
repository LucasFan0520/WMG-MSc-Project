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

Item *find_item(const char *name) {
    Item *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_item(const char *name, int count) {
    Item *p = find_item(name);
    if (p) {
        p->count += count;
    } else {
        Item *newi = malloc(sizeof(Item));
        newi->name = malloc(strlen(name) + 1);
        strcpy(newi->name, name);
        newi->count = count;
        newi->next = head;
        head = newi;
    }
}

void take_item(const char *name, int count) {
    Item *p = find_item(name);
    if (!p) return;
    if (p->count < count) p->count = 0;
    else p->count -= count;
}

void remove_item(const char *name) {
    Item *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void count_item(const char *name) {
    Item *p = find_item(name);
    printf("%d\n", p ? p->count : 0);
}

void report_items(void) {
    Item *p = head;
    while (p) {
        printf("%s %d\n", p->name, p->count);
        p = p->next;
    }
}

void free_all(void) {
    Item *p = head;
    while (p) {
        Item *tmp = p->next;
        free(p->name);
        free(p);
        p = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256];
        int cnt;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s %d", name, &cnt) == 2) add_item(name, cnt);
        } else if (strcmp(cmd, "TAKE") == 0) {
            if (sscanf(rest, "%255s %d", name, &cnt) == 2) take_item(name, cnt);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(rest, "%255s", name) == 1) count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}