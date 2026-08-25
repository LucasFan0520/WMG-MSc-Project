// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef struct Encoded {
    char *name;
    char *encoded;
    struct Encoded *next;
} Encoded;

Encoded *head = NULL, *tail = NULL;

Encoded *find_encoded(const char *name) {
    Encoded *e = head;
    while (e) {
        if (strcmp(e->name, name) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void store_encoded(const char *name, const char *encoded) {
    Encoded *e = malloc(sizeof(Encoded));
    e->name = malloc(strlen(name) + 1);
    strcpy(e->name, name);
    e->encoded = malloc(strlen(encoded) + 1);
    strcpy(e->encoded, encoded);
    e->next = NULL;
    if (tail) {
        tail->next = e;
        tail = e;
    } else {
        head = tail = e;
    }
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->encoded);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    const char *s = e->encoded;
    while (*s) {
        if (!isdigit(*s)) { putchar(*s); s++; continue; }
        long count = 0;
        while (isdigit(*s)) {
            count = count * 10 + (*s - '0');
            if (count > 1000000) { printf("(invalid)\n"); return; }
            s++;
        }
        if (*s == '\0') { printf("(invalid)\n"); return; }
        char ch = *s;
        s++;
        for (long i = 0; i < count; i++) {
            putchar(ch == ' ' ? '_' : ch);
        }
    }
    putchar('\n');
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
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "STORE") == 0) {
            char *name = strtok(NULL, " ");
            char *encoded = strtok(NULL, " ");
            if (name && encoded) store_encoded(name, encoded);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) decode_encoded(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    free(line);
    free_all();
    return 0;
}