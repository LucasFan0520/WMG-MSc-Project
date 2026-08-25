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

Encoded *head = NULL;

Encoded *find_encoded(const char *name) {
    Encoded *e = head;
    while (e) {
        if (strcmp(e->name, name) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void store_encoded(const char *name, const char *encoded) {
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    e->name = malloc(strlen(name) + 1);
    strcpy(e->name, name);
    e->encoded = malloc(strlen(encoded) + 1);
    strcpy(e->encoded, encoded);
    e->next = head;
    head = e;
}

void decode(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    char *p = e->encoded;
    while (*p) {
        if (!isdigit(*p)) { // ignore
            p++;
            continue;
        }
        int count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            p++;
        }
        if (count <= 0) continue;
        char c = *p;
        if (c == 0) break;
        if (count > 1000000) { // treat as invalid
            // just skip? We'll treat as invalid and stop
            break;
        }
        for (int i = 0; i < count; i++) {
            putchar(c == ' ' ? '_' : c);
        }
        p++;
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL;
    Encoded *e = head;
    while (e) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            free(e->name);
            free(e->encoded);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void list_encoded_names(void) {
    Encoded *e = head;
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
    Encoded *e = head;
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
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "STORE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            store_encoded(name, p);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            decode(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_encoded(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded_names();
        }
    }
    free_all();
    return 0;
}