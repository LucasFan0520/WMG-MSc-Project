// F003.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

Alias *table = NULL;
int count = 0;
int capacity = 0;

int find_alias(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void define_a(const char *name, const char *exp) {
    int idx = find_alias(name);
    if (idx != -1) {
        free(table[idx].expansion);
        table[idx].expansion = strdup(exp);
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        table = realloc(table, capacity * sizeof(Alias));
    }
    table[count].name = strdup(name);
    table[count].expansion = strdup(exp);
    count++;
}

void expand_a(const char *name) {
    int idx = find_alias(name);
    if (idx != -1) {
        char *exp = table[idx].expansion;
        for (int i = 0; exp[i] != '\0'; i++) {
            if (exp[i] == ' ') {
                putchar('_');
            } else {
                putchar(exp[i]);
            }
        }
        putchar('\n');
    }
}

void rename_a(const char *old_n, const char *new_n) {
    int idx = find_alias(old_n);
    if (idx != -1) {
        free(table[idx].name);
        table[idx].name = strdup(new_n);
    }
}

void erase_a(const char *name) {
    int idx = find_alias(name);
    if (idx != -1) {
        free(table[idx].name);
        free(table[idx].expansion);
        for (int i = idx; i < count - 1; i++) {
            table[i] = table[i + 1];
        }
        count--;
    }
}

void dump_a(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", table[i].name, table[i].expansion);
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
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = strtok(NULL, " ");
            char *exp = strtok(NULL, "");
            if (name && exp) define_a(name, exp);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) expand_a(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old_n = strtok(NULL, " ");
            char *new_n = strtok(NULL, " ");
            if (old_n && new_n) rename_a(old_n, new_n);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) erase_a(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_a();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].expansion);
    }
    free(table);
    return 0;
}
