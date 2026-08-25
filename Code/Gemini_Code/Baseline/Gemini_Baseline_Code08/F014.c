// F014.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int lbl_count;
    int lbl_capacity;
} Item;

Item *items = NULL;
int count = 0;
int capacity = 0;

int find_item(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void create_item(const char *name, const char *note) {
    int idx = find_item(name);
    if (idx != -1) {
        free(items[idx].note);
        items[idx].note = strdup(note);
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        items = realloc(items, capacity * sizeof(Item));
    }
    items[count].name = strdup(name);
    items[count].note = strdup(note);
    items[count].labels = NULL;
    items[count].lbl_count = 0;
    items[count].lbl_capacity = 0;
    count++;
}

void add_label(const char *name, const char *label) {
    int idx = find_item(name);
    if (idx != -1) {
        Item *it = &items[idx];
        if (it->lbl_count >= it->lbl_capacity) {
            it->lbl_capacity = it->lbl_capacity == 0 ? 4 : it->lbl_capacity * 2;
            it->labels = realloc(it->labels, it->lbl_capacity * sizeof(char *));
        }
        it->labels[it->lbl_count] = strdup(label);
        it->lbl_count++;
    }
}

void remove_label(const char *name, const char *label) {
    int idx = find_item(name);
    if (idx != -1) {
        Item *it = &items[idx];
        for (int i = 0; i < it->lbl_count; i++) {
            if (strcmp(it->labels[i], label) == 0) {
                free(it->labels[i]);
                for (int j = i; j < it->lbl_count - 1; j++) {
                    it->labels[j] = it->labels[j + 1];
                }
                it->lbl_count--;
                break;
            }
        }
    }
}

void delete_item(const char *name) {
    int idx = find_item(name);
    if (idx != -1) {
        free(items[idx].name);
        free(items[idx].note);
        for (int i = 0; i < items[idx].lbl_count; i++) {
            free(items[idx].labels[i]);
        }
        free(items[idx].labels);
        for (int i = idx; i < count - 1; i++) {
            items[i] = items[i + 1];
        }
        count--;
    }
}

void print_item(int idx) {
    printf("%s %s", items[idx].name, items[idx].note);
    for (int i = 0; i < items[idx].lbl_count; i++) {
        printf(" %s", items[idx].labels[i]);
    }
    printf("\n");
}

void find_item_p(const char *name) {
    int idx = find_item(name);
    if (idx != -1) {
        print_item(idx);
    }
}

void report_items(void) {
    for (int i = 0; i < count; i++) {
        print_item(i);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && note) create_item(name, note);
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (name && label) add_label(name, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (name && label) remove_label(name, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_item_p(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].lbl_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
