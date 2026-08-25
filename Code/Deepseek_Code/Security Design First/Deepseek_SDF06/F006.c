// F006.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int count;
} Item;

static Item *items = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_item(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(items[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_item(const char *name, int amount) {
    ssize_t idx = find_item(name);
    if (idx != -1) {
        items[idx].count += amount;
        return;
    }
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Item *new = realloc(items, capacity * sizeof(Item));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        items = new;
    }
    items[count].name = strdup(name);
    if (!items[count].name) { fprintf(stderr, "Memory error\n"); exit(1); }
    items[count].count = amount;
    count++;
}

static void take_item(const char *name, int amount) {
    ssize_t idx = find_item(name);
    if (idx == -1) return;
    if (items[idx].count < amount)
        items[idx].count = 0;
    else
        items[idx].count -= amount;
}

static void remove_item(const char *name) {
    ssize_t idx = find_item(name);
    if (idx == -1) return;
    free(items[idx].name);
    for (size_t i = idx; i < count - 1; i++)
        items[i] = items[i + 1];
    count--;
}

static void count_item(const char *name) {
    ssize_t idx = find_item(name);
    printf("%d\n", idx == -1 ? 0 : items[idx].count);
}

static void report(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %d\n", items[i].name, items[i].count);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++)
        free(items[i].name);
    free(items);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256];
    int amount;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &amount) == 2 && amount > 0)
                add_item(name, amount);
        } else if (strcmp(cmd, "TAKE") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &amount) == 2 && amount > 0)
                take_item(name, amount);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }

    free(line);
    free_all();
    return 0;
}