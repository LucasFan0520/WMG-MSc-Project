/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Encoded {
    char *name;
    char *data;
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

void store_encoded(const char *name, const char *enc) {
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    if (!e) return;
    e->name = strdup(name);
    e->data = strdup(enc);
    e->next = store;
    store = e;
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    const char *p = e->data;
    char *out = malloc(1);
    out[0] = '\0';
    size_t len = 0;
    while (*p) {
        if (!isdigit(*p)) { free(out); return; }
        int count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            if (count > 1000000) { free(out); return; }
            p++;
        }
        if (!*p) { free(out); return; }
        char ch = *p++;
        out = realloc(out, len + count + 1);
        for (int i = 0; i < count; i++) out[len++] = ch;
        out[len] = '\0';
    }
    for (char *c = out; *c; c++) if (*c == ' ') *c = '_';
    printf("%s\n", out);
    free(out);
}

void delete_encoded(const char *name) {
    Encoded *e = store, *prev = NULL;
    while (e) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else store = e->next;
            free(e->name);
            free(e->data);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void list_encoded(void) {
    Encoded *e = store;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (strcmp(cmd, "STORE") == 0 && n >= 3) {
            store_encoded(arg1, arg2);
        } else if (strcmp(cmd, "DECODE") == 0 && n >= 2) {
            decode_encoded(arg1);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_encoded(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    while (store) {
        Encoded *tmp = store;
        store = store->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
    return 0;
}