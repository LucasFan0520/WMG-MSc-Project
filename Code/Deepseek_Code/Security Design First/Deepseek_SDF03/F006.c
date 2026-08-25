// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_ITEM 64

typedef struct Item {
    char name[MAX_ITEM];
    int count;
    struct Item *next;
} Item;

Item *head = NULL;

Item *find_item(const char *name) {
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void free_all() {
    Item *i = head;
    while (i) {
        Item *tmp = i;
        i = i->next;
        free(tmp);
    }
    head = NULL;
}

void handle_add(char *line) {
    char name[MAX_ITEM];
    int count;
    if (sscanf(line, "ADD %63s %d", name, &count) != 2) return;
    if (count <= 0) return;
    Item *i = find_item(name);
    if (i) {
        i->count += count;
    } else {
        Item *newi = malloc(sizeof(Item));
        if (!newi) return;
        strcpy(newi->name, name);
        newi->count = count;
        newi->next = head;
        head = newi;
    }
}

void handle_take(char *line) {
    char name[MAX_ITEM];
    int count;
    if (sscanf(line, "TAKE %63s %d", name, &count) != 2) return;
    if (count <= 0) return;
    Item *i = find_item(name);
    if (!i) return;
    if (i->count >= count) i->count -= count;
    else i->count = 0;
}

void handle_remove(char *line) {
    char name[MAX_ITEM];
    if (sscanf(line, "REMOVE %63s", name) != 1) return;
    Item *i = head, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void handle_count(char *line) {
    char name[MAX_ITEM];
    if (sscanf(line, "COUNT %63s", name) != 1) return;
    Item *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void handle_report() {
    Item *i = head;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "TAKE ", 5) == 0) handle_take(line);
        else if (strncmp(line, "REMOVE ", 7) == 0) handle_remove(line);
        else if (strncmp(line, "COUNT ", 6) == 0) handle_count(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}