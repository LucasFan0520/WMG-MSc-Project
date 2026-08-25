// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Encoded {
    char *name;
    char *data;
    struct Encoded *next;
} Encoded;

Encoded *encoded_list = NULL;

void store_encoded(const char *name, const char *data) {
    Encoded *e = malloc(sizeof(Encoded));
    e->name = strdup(name);
    e->data = strdup(data);
    e->next = encoded_list;
    encoded_list = e;
}

void decode(const char *name) {
    for (Encoded *e = encoded_list; e; e = e->next) {
        if (strcmp(e->name, name) == 0) {
            char *p = e->data;
            while (*p) {
                if (!isdigit((unsigned char)*p)) return;
                long count = strtol(p, &p, 10);
                if (count <= 0 || count > 1000000) return;
                if (!*p) return;
                char c = *p++;
                for (long i = 0; i < count; i++) {
                    putchar(c == ' ' ? '_' : c);
                }
            }
            putchar('\n');
            return;
        }
    }
}

void delete_encoded(const char *name) {
    Encoded **ep = &encoded_list;
    while (*ep) {
        if (strcmp((*ep)->name, name) == 0) {
            Encoded *tmp = *ep;
            *ep = tmp->next;
            free(tmp->name);
            free(tmp->data);
            free(tmp);
            return;
        }
        ep = &(*ep)->next;
    }
}

void list_encoded() {
    for (Encoded *e = encoded_list; e; e = e->next) {
        printf("%s\n", e->name);
    }
}

void cleanup() {
    while (encoded_list) {
        Encoded *tmp = encoded_list->next;
        free(encoded_list->name);
        free(encoded_list->data);
        free(encoded_list);
        encoded_list = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[8192];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "STORE") == 0) {
            if (sscanf(line, "%*s %255s %8191s", a1, a2) == 2) store_encoded(a1, a2);
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) decode(a1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_encoded(a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    cleanup();
    return 0;
}