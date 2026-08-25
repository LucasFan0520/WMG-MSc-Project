/* F038.c */
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

Encoded *find_encoded(char *name) {
    for (Encoded *e = head; e; e = e->next)
        if (strcmp(e->name, name) == 0) return e;
    return NULL;
}

void store_encoded(char *name, char *encoded) {
    Encoded *e = find_encoded(name);
    if (e) {
        free(e->encoded);
        e->encoded = strdup(encoded);
    } else {
        Encoded *new = malloc(sizeof(Encoded));
        new->name = strdup(name);
        new->encoded = strdup(encoded);
        new->next = head;
        head = new;
    }
}

void decode_print(char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    char *p = e->encoded;
    while (*p) {
        // parse decimal count
        if (!isdigit(*p)) { p++; continue; }
        int count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            if (count > 1000000) { // treat as invalid
                count = -1;
                break;
            }
            p++;
        }
        if (count < 0) break;
        // next char is the character
        if (*p == '\0') break;
        char ch = *p++;
        for (int i = 0; i < count; i++) {
            putchar(ch == ' ' ? '_' : ch);
        }
    }
    putchar('\n');
}

void delete_encoded(char *name) {
    Encoded *prev = NULL;
    for (Encoded *e = head; e; e = e->next) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            free(e->name);
            free(e->encoded);
            free(e);
            return;
        }
        prev = e;
    }
}

void list_names(void) {
    for (Encoded *e = head; e; e = e->next)
        printf("%s\n", e->name);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "STORE") == 0 && n == 3) {
            store_encoded(arg1, arg2);
        } else if (strcmp(cmd, "DECODE") == 0 && n == 2) {
            decode_print(arg1);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_encoded(arg1);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_names();
        }
    }
    free(line);
    free_all();
    return 0;
}