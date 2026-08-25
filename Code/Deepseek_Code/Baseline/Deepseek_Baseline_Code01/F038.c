// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct enc {
    char *name;
    char *data;
    struct enc *next;
} Enc;

Enc *head = NULL;
Enc *tail = NULL;

Enc *find_enc(const char *name) {
    Enc *e = head;
    while (e) {
        if (strcmp(e->name, name) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void store_encoded(const char *name, const char *encoded) {
    if (find_enc(name)) return;
    Enc *e = malloc(sizeof(Enc));
    e->name = strdup(name);
    e->data = strdup(encoded);
    e->next = NULL;
    if (tail) tail->next = e;
    else head = e;
    tail = e;
}

void decode_print(const char *name) {
    Enc *e = find_enc(name);
    if (!e) return;
    const char *s = e->data;
    while (*s) {
        char *end;
        long count = strtol(s, &end, 10);
        if (end == s || count <= 0 || count > 10000) { s++; continue; }
        if (*end == '\0') break;
        char ch = *end;
        for (long i = 0; i < count; i++) {
            putchar(ch == ' ' ? '_' : ch);
        }
        s = end + 1;
    }
    putchar('\n');
}

void delete_enc(const char *name) {
    Enc *e = head, *prev = NULL;
    while (e) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            if (e == tail) tail = prev;
            free(e->name);
            free(e->data);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void list_encs(void) {
    Enc *e = head;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], data[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "STORE") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", name, data) == 2)
                store_encoded(name, data);
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                decode_print(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_enc(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encs();
        }
    }
    return 0;
}