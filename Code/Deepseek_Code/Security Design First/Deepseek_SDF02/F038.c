/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct encoded {
    char *name;
    char *encoded;
    struct encoded *next;
} encoded_t;

encoded_t *head = NULL;
encoded_t *tail = NULL;

encoded_t *find_encoded(const char *name) {
    encoded_t *e = head;
    while (e) {
        if (strcmp(e->name, name) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void store_cmd(const char *name, const char *enc) {
    if (find_encoded(name)) return;
    encoded_t *e = malloc(sizeof(encoded_t));
    if (!e) return;
    e->name = strdup(name);
    e->encoded = strdup(enc);
    e->next = NULL;
    if (!e->name || !e->encoded) {
        free(e->name); free(e->encoded); free(e);
        return;
    }
    if (tail) tail->next = e;
    else head = e;
    tail = e;
}

void decode_cmd(const char *name) {
    encoded_t *e = find_encoded(name);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *p = e->encoded;
    while (*p) {
        int count = 0;
        while (*p >= '0' && *p <= '9') {
            count = count * 10 + (*p - '0');
            p++;
            if (count > 1000000) { // treat as invalid
                printf("INVALID\n");
                return;
            }
        }
        if (count == 0) { printf("INVALID\n"); return; }
        char ch = *p;
        if (!ch) { printf("INVALID\n"); return; }
        for (int i = 0; i < count; i++) {
            if (ch == ' ') putchar('_');
            else putchar(ch);
        }
        p++;
    }
    putchar('\n');
}

void delete_cmd(const char *name) {
    encoded_t *prev = NULL, *e = head;
    while (e) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            if (e == tail) tail = prev;
            free(e->name);
            free(e->encoded);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void list_cmd(void) {
    encoded_t *e = head;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

void free_all(void) {
    encoded_t *e = head;
    while (e) {
        encoded_t *nxt = e->next;
        free(e->name);
        free(e->encoded);
        free(e);
        e = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256];
        if (sscanf(line, "%9s %255s", cmd, a) >= 1) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "STORE") == 0) {
                store_cmd(a, rest);
            } else if (strcmp(cmd, "DECODE") == 0) {
                decode_cmd(a);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_cmd(a);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_cmd();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}