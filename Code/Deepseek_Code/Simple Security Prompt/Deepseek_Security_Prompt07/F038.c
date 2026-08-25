// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Encoded {
    char *name;
    char *encoded;
    struct Encoded *next;
} Encoded;

Encoded *store = NULL;

Encoded *find_encoded(const char *name) {
    Encoded *e = store;
    while (e) {
        if (strcmp(e->name, name) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void store_encoded(const char *name, const char *encoded) {
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    e->name = strdup(name);
    e->encoded = strdup(encoded);
    e->next = store;
    store = e;
}

void decode_and_print(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *s = e->encoded;
    int result_len = 0;
    // First pass: validate and compute length, but also check for large counts
    const char *p = s;
    int valid = 1;
    while (*p) {
        if (!isdigit(*p)) { valid = 0; break; }
        long count = strtol(p, (char **)&p, 10);
        if (count > 1000000) { valid = 0; break; } // arbitrary large limit
        if (*p == '\0') { valid = 0; break; }
        char ch = *p;
        p++;
        result_len += (int)count;
        if (result_len > 1000000) { valid = 0; break; }
    }
    if (!valid) {
        printf("INVALID\n");
        return;
    }
    // second pass to print
    p = s;
    while (*p) {
        long count = strtol(p, (char **)&p, 10);
        char ch = *p;
        p++;
        for (int i = 0; i < count; i++) {
            putchar(ch == ' ' ? '_' : ch);
        }
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL;
    Encoded *e = store;
    while (e) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else store = e->next;
            free(e->name);
            free(e->encoded);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void list_encoded_names() {
    Encoded *e = store;
    int first = 1;
    while (e) {
        if (!first) putchar(' ');
        printf("%s", e->name);
        first = 0;
        e = e->next;
    }
    putchar('\n');
}

void free_all() {
    Encoded *e = store;
    while (e) {
        Encoded *next = e->next;
        free(e->name);
        free(e->encoded);
        free(e);
        e = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "STORE") == 0) {
            char name[256], encoded[4096];
            if (sscanf(rest, "%255s %[^\n]", name, encoded) == 2) {
                store_encoded(name, encoded);
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            decode_and_print(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_encoded(rest);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded_names();
        }
    }
    free_all();
    return 0;
}