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

void store_encoded(const char *name, const char *enc) {
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    if (!e) return;
    e->name = strdup(name);
    e->encoded = strdup(enc);
    e->next = NULL;
    if (!e->name || !e->encoded) {
        free(e->name); free(e->encoded); free(e);
        return;
    }
    if (!head) head = e;
    else {
        Encoded *last = head;
        while (last->next) last = last->next;
        last->next = e;
    }
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    const char *p = e->encoded;
    while (*p) {
        // parse decimal count
        long count = 0;
        while (*p >= '0' && *p <= '9') {
            count = count * 10 + (*p - '0');
            p++;
            if (count > 1000000) { // treat as invalid
                return;
            }
        }
        if (count == 0) {
            // invalid
            return;
        }
        if (!*p) return; // missing char
        char ch = *p;
        p++;
        for (long i = 0; i < count; i++) {
            putchar(ch == ' ' ? '_' : ch);
        }
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *e = head, *prev = NULL;
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], enc[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "STORE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, enc) == 2)
                store_encoded(name, enc);
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                decode_encoded(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    free(line);
    free_all();
    return 0;
}