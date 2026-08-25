// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Encoded {
    char *name;
    char *data; // encoded string
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
    if (!e) return;
    e->name = strdup(name);
    e->data = strdup(encoded);
    e->next = head;
    head = e;
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    const char *s = e->data;
    while (*s) {
        if (!isdigit(*s)) {
            // invalid, ignore
            return;
        }
        long count = strtol(s, (char **)&s, 10);
        if (count <= 0 || count > 10000) return; // treat as invalid
        if (!*s) break;
        char ch = *s++;
        for (long i = 0; i < count; i++) {
            putchar(ch);
        }
    }
    // No newline; spec doesn't say, but we add a newline
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
            free(e->data);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void list_encoded_names(void) {
    Encoded *e = head;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

void free_all(void) {
    while (head) {
        Encoded *next = head->next;
        free(head->name);
        free(head->data);
        free(head);
        head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "STORE") == 0) {
            char name[256], encoded[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, encoded) == 2) {
                store_encoded(name, encoded);
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                decode_encoded(name);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_encoded(name);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded_names();
        }
    }

    free(line);
    free_all();
    return 0;
}