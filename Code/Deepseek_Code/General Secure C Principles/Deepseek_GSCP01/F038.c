/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Encoded {
    char *name;
    char *encoded;
    struct Encoded *next;
} Encoded;

Encoded *encodings = NULL;

Encoded *find_encoded(const char *name) {
    Encoded *e = encodings;
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
    e->next = encodings;
    encodings = e;
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) { printf("NOT_FOUND\n"); return; }
    const char *p = e->encoded;
    while (*p) {
        if (!isdigit(*p)) { p++; continue; }
        int count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            if (count > 1000000) { printf("INVALID\n"); return; }
            p++;
        }
        if (!*p) { printf("INVALID\n"); return; }
        char ch = *p;
        for (int i = 0; i < count; i++) {
            putchar(ch);
        }
        p++;
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *e = encodings, *prev = NULL;
    while (e && strcmp(e->name, name) != 0) {
        prev = e;
        e = e->next;
    }
    if (!e) return;
    if (prev) prev->next = e->next;
    else encodings = e->next;
    free(e->name);
    free(e->encoded);
    free(e);
}

void list_encoded(void) {
    Encoded *e = encodings;
    int first = 1;
    while (e) {
        if (!first) putchar(' ');
        printf("%s", e->name);
        first = 0;
        e = e->next;
    }
    putchar('\n');
}

void free_all(void) {
    Encoded *e = encodings;
    while (e) {
        Encoded *next = e->next;
        free(e->name);
        free(e->encoded);
        free(e);
        e = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "STORE") == 0) {
            char name[256];
            char *p = line + 6;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            store_encoded(name, p);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            decode_encoded(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_encoded(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    free_all();
    free(line);
    return 0;
}