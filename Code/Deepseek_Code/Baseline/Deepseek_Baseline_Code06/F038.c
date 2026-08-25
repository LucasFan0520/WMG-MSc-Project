/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Encoded {
    char *name;
    char *decoded;
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

int decode_string(const char *encoded, char **out) {
    int len = strlen(encoded);
    char *dec = malloc(1);
    dec[0] = '\0';
    int dec_len = 0;
    int i = 0;
    while (i < len) {
        if (!isdigit(encoded[i])) { free(dec); return 0; }
        long count = 0;
        while (i < len && isdigit(encoded[i])) {
            count = count * 10 + (encoded[i] - '0');
            if (count > 10000) { free(dec); return 0; }
            i++;
        }
        if (i >= len || count <= 0) { free(dec); return 0; }
        char ch = encoded[i++];
        dec = realloc(dec, dec_len + count + 1);
        for (int j = 0; j < count; j++) dec[dec_len++] = ch;
        dec[dec_len] = '\0';
    }
    *out = dec;
    return 1;
}

void store_encoded(const char *name, const char *encoded) {
    if (find_encoded(name)) return;
    char *decoded;
    if (!decode_string(encoded, &decoded)) return;
    Encoded *e = malloc(sizeof(Encoded));
    e->name = strdup(name);
    e->decoded = decoded;
    e->next = head;
    head = e;
}

void decode_and_print(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    for (char *c = e->decoded; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
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
            free(e->decoded);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void list_encoded() {
    Encoded *e = head;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

void free_all() {
    while (head) {
        Encoded *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->decoded);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], enc[4096];
        int n = sscanf(line, "%31s %255s %s", cmd, name, enc);
        if (strcmp(cmd, "STORE") == 0 && n == 3) {
            store_encoded(name, enc);
        } else if (strcmp(cmd, "DECODE") == 0 && n == 2) {
            decode_and_print(name);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_encoded();
        }
    }
    free_all();
    return 0;
}