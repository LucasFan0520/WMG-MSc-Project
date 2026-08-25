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

Encoded *store = NULL;

Encoded *find_encoded(const char *name) {
    Encoded *e = store;
    while (e) {
        if (strcmp(e->name, name) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void free_encoded(Encoded *e) {
    free(e->name);
    free(e->encoded);
    free(e);
}

void store_encoded(const char *name, const char *enc) {
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    e->name = strdup(name);
    e->encoded = strdup(enc);
    e->next = store;
    store = e;
}

void decode_print(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
    char *p = e->encoded;
    while (*p) {
        char *end;
        long count = strtol(p, &end, 10);
        if (end == p || count <= 0 || count > 10000) {
            printf("ERROR\n");
            return;
        }
        if (*end == '\0') {
            printf("ERROR\n");
            return;
        }
        char ch = *end;
        for (long i = 0; i < count; i++) {
            putchar(ch == ' ' ? '_' : ch);
        }
        p = end + 1;
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL, *curr = store;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else store = curr->next;
            free_encoded(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_encoded(void) {
    Encoded *e = store;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "STORE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            store_encoded(name, p);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) decode_print(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }

    while (store) {
        Encoded *tmp = store;
        store = store->next;
        free_encoded(tmp);
    }
    return 0;
}