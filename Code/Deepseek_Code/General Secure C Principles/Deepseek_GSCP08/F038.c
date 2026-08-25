// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Encoded { char *name; char *encoded; struct Encoded *next; } Encoded;
Encoded *store = NULL;

Encoded *find_encoded(const char *name) {
    for (Encoded *e = store; e; e = e->next)
        if (strcmp(e->name, name) == 0) return e;
    return NULL;
}

void store_encoded(const char *name, const char *enc) {
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    if (!e) return;
    e->name = strdup(name);
    e->encoded = strdup(enc);
    e->next = store;
    store = e;
}

void decode_print(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) { printf("NOT_FOUND\n"); return; }
    const char *s = e->encoded;
    while (*s) {
        if (!isdigit(*s)) { printf("INVALID\n"); return; }
        int count = 0;
        while (isdigit(*s)) { count = count * 10 + (*s - '0'); s++; }
        if (count <= 0 || count > 10000) { printf("INVALID\n"); return; } // large count invalid
        if (!*s) { printf("INVALID\n"); return; }
        char ch = *s++;
        for (int i = 0; i < count; i++) putchar(ch == ' ' ? '_' : ch);
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL;
    for (Encoded *e = store; e; prev = e, e = e->next) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next; else store = e->next;
            free(e->name); free(e->encoded); free(e);
            return;
        }
    }
}

void list_encoded_names(void) {
    for (Encoded *e = store; e; e = e->next)
        printf("%s\n", e->name);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "STORE") == 0) {
            char name[256], enc[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *e = rest + strlen(name);
                while (*e == ' ') e++;
                strcpy(enc, e);
                store_encoded(name, enc);
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) decode_print(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded_names();
        }
    }
    while (store) { Encoded *e = store; store = e->next; free(e->name); free(e->encoded); free(e); }
    return 0;
}