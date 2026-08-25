// F006.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InvItem;

InvItem *inv = NULL;
int count_items = 0;
int capacity = 0;

int find_item(const char *name) {
    for (int i = 0; i < count_items; i++) {
        if (strcmp(inv[i].item, name) == 0) {
            return i;
        }
    }
    return -1;
}

void add_i(const char *name, int amt) {
    int idx = find_item(name);
    if (idx != -1) {
        inv[idx].count += amt;
        return;
    }
    if (count_items >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        inv = realloc(inv, capacity * sizeof(InvItem));
    }
    inv[count_items].item = strdup(name);
    inv[count_items].count = amt;
    count_items++;
}

void take_i(const char *name, int amt) {
    int idx = find_item(name);
    if (idx != -1) {
        inv[idx].count -= amt;
        if (inv[idx].count < 0) {
            inv[idx].count = 0;
        }
    }
}

void remove_i(const char *name) {
    int idx = find_item(name);
    if (idx != -1) {
        free(inv[idx].item);
        for (int i = idx; i < count_items - 1; i++) {
            inv[i] = inv[i + 1];
        }
        count_items--;
    }
}

void count_i(const char *name) {
    int idx = find_item(name);
    if (idx != -1) {
        printf("%d\n", inv[idx].count);
    } else {
        printf("0\n");
    }
}

void report_i(void) {
    for (int i = 0; i < count_items; i++) {
        printf("%s %d\n", inv[i].item, inv[i].count);
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
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *cnt_s = strtok(NULL, " ");
            if (name && cnt_s) add_i(name, atoi(cnt_s));
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *name = strtok(NULL, " ");
            char *cnt_s = strtok(NULL, " ");
            if (name && cnt_s) take_i(name, atoi(cnt_s));
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) remove_i(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) count_i(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_i();
        }
    }
    free(line);
    for (int i = 0; i < count_items; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
