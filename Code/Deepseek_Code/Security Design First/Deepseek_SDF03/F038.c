// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_ENCODED 4096

typedef struct Encoded {
    char name[MAX_NAME];
    char encoded[MAX_ENCODED];
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

void free_all() {
    Encoded *e = head;
    while (e) {
        Encoded *tmp = e;
        e = e->next;
        free(tmp);
    }
    head = NULL;
}

int decode_rle(const char *encoded, char *decoded, size_t max_len) {
    decoded[0] = '\0';
    size_t dlen = 0;
    const char *p = encoded;
    while (*p) {
        if (!isdigit(*p)) return 0;
        int count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            if (count > 1000000) return 0; // too large
            p++;
        }
        if (!*p) return 0;
        char ch = *p++;
        for (int i = 0; i < count; i++) {
            if (dlen + 1 >= max_len) return 0;
            decoded[dlen++] = ch;
        }
    }
    decoded[dlen] = '\0';
    return 1;
}

void handle_store(char *line) {
    char name[MAX_NAME], encoded[MAX_ENCODED];
    int n = sscanf(line, "STORE %63s %4095[^\n]", name, encoded);
    if (n < 2) return;
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    if (!e) return;
    strcpy(e->name, name);
    strcpy(e->encoded, encoded);
    e->next = head;
    head = e;
}

void handle_decode(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DECODE %63s", name) != 1) return;
    Encoded *e = find_encoded(name);
    if (!e) return;
    char decoded[MAX_LINE];
    if (!decode_rle(e->encoded, decoded, MAX_LINE)) return;
    for (char *c = decoded; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void handle_delete(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETE %63s", name) != 1) return;
    Encoded *e = head, *prev = NULL;
    while (e) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            free(e);
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
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "STORE ", 6) == 0) handle_store(line);
        else if (strncmp(line, "DECODE ", 7) == 0) handle_decode(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}