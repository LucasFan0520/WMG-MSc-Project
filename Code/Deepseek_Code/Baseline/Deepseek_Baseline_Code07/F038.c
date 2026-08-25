// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Encoded {
    char *name;
    char *encoded;
    struct Encoded *next;
} Encoded;

Encoded *head = NULL;
Encoded *tail = NULL;

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
    e->name = malloc(strlen(name)+1);
    strcpy(e->name, name);
    e->encoded = malloc(strlen(encoded)+1);
    strcpy(e->encoded, encoded);
    e->next = NULL;
    if (tail) tail->next = e;
    else head = e;
    tail = e;
}

void decode_print(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    const char *p = e->encoded;
    while (*p) {
        if (*p < '0' || *p > '9') { p++; continue; }
        long count = 0;
        while (*p >= '0' && *p <= '9') {
            count = count * 10 + (*p - '0');
            if (count > 1000000) { printf("INVALID\n"); return; }
            p++;
        }
        if (*p == 0) { printf("INVALID\n"); return; }
        char ch = *p;
        p++;
        if (count > 10000) { printf("INVALID\n"); return; } // arbitrary large
        for (long i = 0; i < count; i++) {
            if (ch == ' ') putchar('_');
            else putchar(ch);
        }
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
            if (e == tail) tail = prev;
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
    Encoded *e = head;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
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

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "STORE") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *enc = strtok(NULL, "");
            if (!enc) enc = "";
            store_encoded(name, enc);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = strtok(rest, " ");
            if (name) decode_print(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    free_all();
    return 0;
}