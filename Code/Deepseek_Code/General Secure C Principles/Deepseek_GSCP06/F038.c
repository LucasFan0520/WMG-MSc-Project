// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    e->name = strdup(name);
    e->encoded = strdup(enc);
    e->next = store;
    store = e;
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    const char *p = e->encoded;
    while (*p) {
        int count = 0;
        while (*p >= '0' && *p <= '9') {
            count = count * 10 + (*p - '0');
            p++;
        }
        if (count == 0) break; // invalid
        if (count > 1000000) { printf("invalid\n"); return; } // treat very large as invalid
        char ch = *p;
        if (ch == 0) break;
        p++;
        for (int i = 0; i < count; i++) {
            if (ch == ' ') putchar('_');
            else putchar(ch);
        }
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *cur = store, *prev = NULL;
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

void cleanup_encoded(void) {
    Encoded *e = store;
    while (e) {
        Encoded *tmp = e;
        e = e->next;
        free(tmp->name);
        free(tmp->encoded);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], rest[4096];
        int n = sscanf(line, "%31s %255s %4095s", cmd, name, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "STORE") == 0 && n >= 3) {
            store_encoded(name, rest);
        } else if (strcmp(cmd, "DECODE") == 0) {
            decode_encoded(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    cleanup_encoded();
    return 0;
}