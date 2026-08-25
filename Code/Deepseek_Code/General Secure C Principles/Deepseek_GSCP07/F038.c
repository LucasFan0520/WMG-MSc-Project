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

Encoded* find_encoded(const char *name) {
    Encoded *e = head;
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

int decode(const char *encoded, char **out) {
    // Output will be a string (decoded), may be large but we'll allocate.
    // We'll first compute length to allocate.
    size_t out_len = 0;
    const char *p = encoded;
    while (*p) {
        if (!isdigit(*p)) return 0; // must start with digit
        int count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            p++;
            if (count > 1000000) return 0; // very large, treat as invalid
        }
        if (*p == '\0') return 0; // missing character after count
        char ch = *p;
        p++;
        out_len += count;
    }
    char *result = malloc(out_len + 1);
    char *r = result;
    p = encoded;
    while (*p) {
        int count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            p++;
        }
        char ch = *p;
        p++;
        for (int i = 0; i < count; i++) {
            *r++ = ch;
        }
    }
    *r = '\0';
    *out = result;
    return 1;
}

void handle_store(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    if (find_encoded(name)) return;
    char *enc = strtok(NULL, " ");
    if (!enc) return;
    // Validate that it can be decoded
    char *dummy;
    if (!decode(enc, &dummy)) return;
    free(dummy);
    Encoded *new = malloc(sizeof(Encoded));
    new->name = strdup(name);
    new->encoded = strdup(enc);
    new->next = NULL;
    if (!head) head = new;
    else {
        Encoded *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_decode(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Encoded *e = find_encoded(name);
    if (!e) return;
    char *decoded;
    if (!decode(e->encoded, &decoded)) return;
    for (char *s = decoded; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
    free(decoded);
}

void handle_delete(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Encoded *e = head, *prev = NULL;
    while (e) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            free_encoded(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void handle_list() {
    Encoded *e = head;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "STORE") == 0) handle_store(cmd);
        else if (strcmp(cmd, "DECODE") == 0) handle_decode(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "LIST") == 0) handle_list();
    }
    while (head) {
        Encoded *tmp = head;
        head = head->next;
        free_encoded(tmp);
    }
    return 0;
}