// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct item {
    char *name;
    int count;
    struct item *next;
} Item;

Item *head = NULL;
Item *tail = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Item *find_item(const char *name) {
    Item *it = head;
    while (it) {
        if (strcmp(it->name, name) == 0) return it;
        it = it->next;
    }
    return NULL;
}

void add_item(const char *name, int count) {
    Item *it = find_item(name);
    if (it) {
        it->count += count;
        return;
    }
    Item *newi = malloc(sizeof(Item));
    if (!newi) { fprintf(stderr, "Memory error\n"); exit(1); }
    newi->name = strdup(name);
    if (!newi->name) { fprintf(stderr, "Memory error\n"); exit(1); }
    newi->count = count;
    newi->next = NULL;
    if (tail) tail->next = newi;
    else head = newi;
    tail = newi;
}

void take_item(const char *name, int count) {
    Item *it = find_item(name);
    if (!it) return;
    it->count -= count;
    if (it->count < 0) it->count = 0;
}

void remove_item(const char *name) {
    Item *it = head, *prev = NULL;
    while (it) {
        if (strcmp(it->name, name) == 0) {
            if (prev) prev->next = it->next;
            else head = it->next;
            if (it == tail) tail = prev;
            free(it->name);
            free(it);
            return;
        }
        prev = it;
        it = it->next;
    }
}

void count_item(const char *name) {
    Item *it = find_item(name);
    printf("%d\n", it ? it->count : 0);
}

void report_items(void) {
    Item *it = head;
    while (it) {
        printf("%s %d\n", it->name, it->count);
        it = it->next;
    }
}

void free_all(void) {
    Item *it = head;
    while (it) {
        Item *next = it->next;
        free(it->name);
        free(it);
        it = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report_items(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "TAKE") == 0) {
            char *name = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            int count = atoi(rest);
            if (strcmp(cmd, "ADD") == 0) add_item(name, count);
            else take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_item(rest);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_item(rest);
        }
    }
    free_all();
    return 0;
}