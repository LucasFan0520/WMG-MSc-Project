// F038.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Encoded {
    char *name;
    char *encoded;
} Encoded;

static Encoded *encodings = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_encoded(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(encodings[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void store_encoded(const char *name, const char *encoded) {
    if (find_encoded(name) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Encoded *new = realloc(encodings, capacity * sizeof(Encoded));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        encodings = new;
    }
    encodings[count].name = strdup(name);
    encodings[count].encoded = strdup(encoded);
    if (!encodings[count].name || !encodings[count].encoded) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void decode_and_print(const char *name) {
    ssize_t idx = find_encoded(name);
    if (idx == -1) return;
    const char *p = encodings[idx].encoded;
    char *out = malloc(1);
    size_t out_len = 0;
    size_t out_cap = 1;
    while (*p) {
        char *end;
        unsigned long count = strtoul(p, &end, 10);
        if (p == end) break; // no number
        if (count > 1000000) { // treat as invalid
            free(out);
            return;
        }
        p = end;
        if (!*p) break; // no character after number
        char ch = *p;
        p++;
        // append ch count times
        while (out_len + count + 1 > out_cap) {
            out_cap = out_cap * 2 + count;
            char *new_out = realloc(out, out_cap);
            if (!new_out) { fprintf(stderr, "Memory error\n"); exit(1); }
            out = new_out;
        }
        for (unsigned long i = 0; i < count; i++)
            out[out_len++] = ch;
    }
    out[out_len] = '\0';
    for (char *q = out; *q; q++)
        putchar(*q == ' ' ? '_' : *q);
    putchar('\n');
    free(out);
}

static void delete_encoded(const char *name) {
    ssize_t idx = find_encoded(name);
    if (idx == -1) return;
    free(encodings[idx].name);
    free(encodings[idx].encoded);
    for (size_t i = idx; i < count - 1; i++)
        encodings[i] = encodings[i + 1];
    count--;
}

static void list_encoded_names(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s\n", encodings[i].name);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(encodings[i].name);
        free(encodings[i].encoded);
    }
    free(encodings);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], encoded[8192];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "STORE") == 0) {
            if (sscanf(line, "%*s %255s %8191[^\n]", name, encoded) == 2)
                store_encoded(name, encoded);
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                decode_and_print(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded_names();
        }
    }

    free(line);
    free_all();
    return 0;
}