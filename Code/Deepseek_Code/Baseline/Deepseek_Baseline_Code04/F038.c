/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Encoded {
    char *name;
    char *encoded;
    struct Encoded *next;
} Encoded;

Encoded *encoded_list = NULL;

Encoded *find_encoded(const char *name) {
    Encoded *e = encoded_list;
    while (e) {
        if (strcmp(e->name, name) == 0) return e;
        e = e->next;
    }
    return NULL;
}

int is_valid_encoded(const char *s) {
    // check format: digits then char, repeat
    while (*s) {
        if (*s < '0' || *s > '9') return 0;
        while (*s >= '0' && *s <= '9') s++;
        if (*s == '\0') return 0;
        s++; // skip char
    }
    return 1;
}

int decode_encoded(const char *encoded, char **out) {
    if (!is_valid_encoded(encoded)) return 0;
    int total_len = 0;
    const char *p = encoded;
    while (*p) {
        int count = 0;
        while (*p >= '0' && *p <= '9') {
            count = count * 10 + (*p - '0');
            if (count > 10000) return 0; // reject huge counts
            p++;
        }
        if (count < 0) return 0;
        total_len += count;
        p++; // skip char
    }
    char *result = malloc(total_len + 1);
    char *r = result;
    p = encoded;
    while (*p) {
        int count = 0;
        while (*p >= '0' && *p <= '9') {
            count = count * 10 + (*p - '0');
            p++;
        }
        char c = *p++;
        for (int i = 0; i < count; i++) *r++ = c;
    }
    *r = '\0';
    *out = result;
    return 1;
}

void store_encoded(const char *name, const char *encoded) {
    if (find_encoded(name)) return;
    if (!is_valid_encoded(encoded)) return;
    Encoded *e = malloc(sizeof(Encoded));
    e->name = strdup(name);
    e->encoded = strdup(encoded);
    e->next = encoded_list;
    encoded_list = e;
}

void decode_and_print(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    char *decoded;
    if (!decode_encoded(e->encoded, &decoded)) return;
    for (char *p = decoded; *p; p++) if (*p == ' ') *p = '_';
    printf("%s\n", decoded);
    free(decoded);
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL, *cur = encoded_list;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else encoded_list = cur->next;
            free(cur->name);
            free(cur->encoded);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_encoded_names(void) {
    Encoded *e = encoded_list;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

void free_encoded_list(void) {
    while (encoded_list) {
        Encoded *tmp = encoded_list;
        encoded_list = encoded_list->next;
        free(tmp->name);
        free(tmp->encoded);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256];
        int n = sscanf(line, "%s %s", cmd, name);
        if (strcmp(cmd, "STORE") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            if (*p) store_encoded(name, p);
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (n < 2) continue;
            decode_and_print(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded_names();
        }
    }
    free_encoded_list();
    return 0;
}