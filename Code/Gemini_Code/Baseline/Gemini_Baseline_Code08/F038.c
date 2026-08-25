// F038.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

RLEString *store = NULL;
int count = 0;
int capacity = 0;

int find_string(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(store[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void store_string(const char *name, const char *encoded) {
    if (find_string(name) != -1) return;
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        store = realloc(store, capacity * sizeof(RLEString));
    }
    store[count].name = strdup(name);
    store[count].encoded = strdup(encoded);
    count++;
}

void decode_string(const char *name) {
    int idx = find_string(name);
    if (idx == -1) return;
    const char *p = store[idx].encoded;
    int invalid = 0;
    while (*p != '\0') {
        if (isdigit(*p)) {
            char *endptr;
            long run_len = strtol(p, &endptr, 10);
            if (run_len > 10000 || run_len < 0) {
                invalid = 1;
                break;
            }
            p = endptr;
            if (*p == '\0') {
                invalid = 1;
                break;
            }
            p++;
        } else {
            p++;
        }
    }
    if (invalid) {
        printf("INVALID\n");
        return;
    }
    p = store[idx].encoded;
    while (*p != '\0') {
        if (isdigit(*p)) {
            char *endptr;
            long run_len = strtol(p, &endptr, 10);
            char c = *endptr;
            for (long i = 0; i < run_len; i++) {
                if (c == ' ') {
                    putchar('_');
                } else {
                    putchar(c);
                }
            }
            p = endptr + 1;
        } else {
            if (*p == ' ') {
                putchar('_');
            } else {
                putchar(*p);
            }
            p++;
        }
    }
    putchar('\n');
}

void delete_string(const char *name) {
    int idx = find_string(name);
    if (idx != -1) {
        free(store[idx].name);
        free(store[idx].encoded);
        for (int i = idx; i < count - 1; i++) {
            store[i] = store[i + 1];
        }
        count--;
    }
}

void list_strings(void) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", store[i].name);
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
        if (strcmp(cmd, "STORE") == 0) {
            char *name = strtok(NULL, " ");
            char *encoded = strtok(NULL, "");
            if (name && encoded) store_string(name, encoded);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = strtok(NULL, "");
            if (name) decode_string(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_string(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_strings();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    return 0;
}
