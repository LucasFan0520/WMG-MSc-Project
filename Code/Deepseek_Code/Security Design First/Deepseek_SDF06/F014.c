// F014.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **labels;
    size_t label_count;
    size_t label_cap;
} LabelList;

typedef struct {
    char *name;
    char *note;
    LabelList labels;
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

static void add_label(LabelList *ll, const char *label) {
    if (ll->label_count == ll->label_cap) {
        ll->label_cap = ll->label_cap ? ll->label_cap * 2 : 4;
        char **new = realloc(ll->labels, ll->label_cap * sizeof(char *));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        ll->labels = new;
    }
    ll->labels[ll->label_count] = strdup(label);
    if (!ll->labels[ll->label_count]) { fprintf(stderr, "Memory error\n"); exit(1); }
    ll->label_count++;
}

static void init_labels(LabelList *ll) {
    ll->labels = NULL;
    ll->label_count = 0;
    ll->label_cap = 0;
}

static void free_labels(LabelList *ll) {
    for (size_t i = 0; i < ll->label_count; i++)
        free(ll->labels[i]);
    free(ll->labels);
}

static void add_item(const char *name, const char *note) {
    if (find_item(name) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Item *new = realloc(items, capacity * sizeof(Item));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        items = new;
    }
    items[count].name = strdup(name);
    items[count].note = strdup(note);
    if (!items[count].name || !items[count].note) { fprintf(stderr, "Memory error\n"); exit(1); }
    init_labels(&items[count].labels);
    count++;
}

static void label_item(const char *name, const char *label) {
    ssize_t idx = find_item(name);
    if (idx == -1) return;
    add_label(&items[idx].labels, label);
}

static void unlabel_item(const char *name, const char *label) {
    ssize_t idx = find_item(name);
    if (idx == -1) return;
    LabelList *ll = &items[idx].labels;
    for (size_t i = 0; i < ll->label_count; i++) {
        if (strcmp(ll->labels[i], label) == 0) {
            free(ll->labels[i]);
            for (size_t j = i; j < ll->label_count - 1; j++)
                ll->labels[j] = ll->labels[j + 1];
            ll->label_count--;
            return;
        }
    }
}

static void delete_item(const char *name) {
    ssize_t idx = find_item(name);
    if (idx == -1) return;
    free(items[idx].name);
    free(items[idx].note);
    free_labels(&items[idx].labels);
    for (size_t i = idx; i < count - 1; i++)
        items[i] = items[i + 1];
    count--;
}

static void find_item_print(const char *name) {
    ssize_t idx = find_item(name);
    if (idx == -1) return;
    printf("%s %s", items[idx].name, items[idx].note);
    for (size_t i = 0; i < items[idx].labels.label_count; i++)
        printf(" %s", items[idx].labels.labels[i]);
    putchar('\n');
}

static void report_items(void) {
    for (size_t i = 0; i < count; i++) {
        printf("%s %s", items[i].name, items[i].note);
        for (size_t j = 0; j < items[i].labels.label_count; j++)
            printf(" %s", items[i].labels.labels[j]);
        putchar('\n');
    }
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        free_labels(&items[i].labels);
    }
    free(items);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], arg[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ITEM") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, arg) == 2)
                add_item(name, arg);
        } else if (strcmp(cmd, "LABEL") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, arg) == 2)
                label_item(name, arg);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, arg) == 2)
                unlabel_item(name, arg);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }

    free(line);
    free_all();
    return 0;
}