/*
 * F038.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct encoded {
    char *name;
    char *encoded;
    struct encoded *next;
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

void store_encoded(const char *name, const char *enc) {
    if (find_encoded(name)) return;
    // validate: ensure no huge counts (e.g., > 1000)
    int valid = 1;
    const char *p = enc;
    while (*p) {
        if (!isdigit(*p)) { valid = 0; break; }
        int count = 0;
        while (isdigit(*p)) { count = count*10 + (*p-'0'); p++; }
        if (count > 10000) { valid = 0; break; }
        if (*p == 0) { valid = 0; break; } // need a char after count
        p++; // skip the char
    }
    if (!valid) return;
    Encoded *e = malloc(sizeof(Encoded));
    e->name = strdup(name);
    e->encoded = strdup(enc);
    e->next = encodings;
    encodings = e;
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *p = e->encoded;
    while (*p) {
        int count = 0;
        while (isdigit(*p)) { count = count*10 + (*p-'0'); p++; }
        char ch = *p;
        if (ch == 0) break;
        p++;
        for (int i = 0; i < count; i++) {
            putchar(ch == ' ' ? '_' : ch);
        }
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL, *e = encodings;
    while (e) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else encodings = e->next;
            free(e->name);
            free(e->encoded);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void list_encoded(void) {
    Encoded *e = encodings;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256];
        if (sscanf(line, "%31s %255s", cmd, name) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(name) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "STORE") == 0) {
            store_encoded(name, rest);
        } else if (strcmp(cmd, "DECODE") == 0) {
            decode_encoded(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    return 0;
}