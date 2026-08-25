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

void store_encoded(const char *name, const char *enc) {
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    if (!e) return;
    e->name = strdup(name);
    e->encoded = strdup(enc);
    e->next = store;
    store = e;
}

void decode_and_print(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    const char *s = e->encoded;
    while (*s) {
        if (!isdigit(*s)) { s++; continue; }
        int count = 0;
        while (isdigit(*s)) {
            count = count * 10 + (*s - '0');
            if (count > 1000000) { // large count invalid
                printf("invalid\n");
                return;
            }
            s++;
        }
        if (*s == '\0') break;
        char ch = *s++;
        for (int i = 0; i < count; i++) {
            putchar(ch == ' ' ? '_' : ch);
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "STORE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *enc = space + 1;
            while (*enc == ' ') enc++;
            store_encoded(name, enc);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            decode_and_print(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    free_all();
    return 0;
}