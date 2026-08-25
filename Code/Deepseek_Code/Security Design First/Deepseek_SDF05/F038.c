// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void store_encoded(const char *name, const char *enc) {
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    if (!e) return;
    e->name = safe_strdup(name);
    e->encoded = safe_strdup(enc);
    e->next = store;
    store = e;
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) { printf("NOT_FOUND\n"); return; }
    const char *p = e->encoded;
    while (*p) {
        if (!isdigit(*p)) { /* invalid */ break; }
        long count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            if (count > 1000000) { /* very large, treat invalid */ break; }
            p++;
        }
        if (count > 1000000) { printf("INVALID\n"); return; }
        if (*p == '\0') break; // missing char
        char c = *p;
        p++;
        for (long i = 0; i < count; i++) {
            putchar(c == ' ' ? '_' : c);
        }
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL, *cur = store;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else store = cur->next;
            free(cur->name);
            free(cur->encoded);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_encoded(void) {
    Encoded *e = store;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

void free_all(void) {
    Encoded *e = store;
    while (e) {
        Encoded *next = e->next;
        free(e->name);
        free(e->encoded);
        free(e);
        e = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "STORE") == 0 && n >= 3) {
            store_encoded(arg1, arg2);
        } else if (strcmp(cmd, "DECODE") == 0 && n >= 2) {
            decode_encoded(arg1);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_encoded(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    free_all();
    return 0;
}