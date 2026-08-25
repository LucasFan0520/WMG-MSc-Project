/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Encoded {
    char *name;
    char *encoded;
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
    e->encoded = strdup(encoded);
    e->next = head;
    head = e;
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
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
        }
        if (count == 0) {
            // invalid
            printf("INVALID\n");
            return;
        }
        if (*p == '\0') {
            printf("INVALID\n");
            return;
        }
        char ch = *p++;
        // check if count is too large, e.g., > 1000 to avoid excessive output
        if (count > 1000) {
            printf("INVALID\n");
            return;
        }
        for (int i = 0; i < count; i++) {
            if (ch == ' ') putchar('_');
            else putchar(ch);
        }
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->encoded);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_encoded_names(void) {
    Encoded *e = head;
    int first = 1;
    while (e) {
        if (!first) printf(" ");
        printf("%s", e->name);
        first = 0;
        e = e->next;
    }
    printf("\n");
}

void free_all(void) {
    Encoded *e = head;
    while (e) {
        Encoded *tmp = e;
        e = e->next;
        free(tmp->name);
        free(tmp->encoded);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "STORE") == 0) {
            char name[256], encoded[512];
            if (sscanf(line + 6, "%255s %511s", name, encoded) == 2) {
                store_encoded(name, encoded);
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            decode_encoded(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_encoded(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded_names();
        }
    }
    free_all();
    return 0;
}