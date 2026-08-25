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

Encoded *head = NULL;

Encoded *find_encoded(char *name) {
    for (Encoded *e = head; e; e = e->next) {
        if (strcmp(e->name, name) == 0) return e;
    }
    return NULL;
}

void store_encoded(char *name, char *data) {
    Encoded *e = find_encoded(name);
    if (!e) {
        e = malloc(sizeof(Encoded));
        e->name = strdup(name);
        e->next = head;
        head = e;
    } else {
        free(e->data);
    }
    e->data = strdup(data);
}

void decode_encoded(char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    char *p = e->data;
    while (*p) {
        if (!isdigit((unsigned char)*p)) {
            p++;
            continue;
        }
        long count = strtol(p, &p, 10);
        if (count <= 0 || count > 1000000 || !*p) break;
        char c = *p++;
        for (long i = 0; i < count; i++) {
            putchar(c == ' ' ? '_' : c);
        }
    }
    putchar('\n');
}

void delete_encoded(char *name) {
    Encoded *prev = NULL;
    Encoded *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->data);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_encoded() {
    for (Encoded *e = head; e; e = e->next) {
        printf("%s\n", e->name);
    }
}

int main() {
    char cmd[20], a[100], data[2000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "STORE") == 0) {
            scanf("%99s", a);
            fgets(data, sizeof(data), stdin);
            data[strcspn(data, "\n")] = 0;
            store_encoded(a, data);
        } else if (strcmp(cmd, "DECODE") == 0) {
            scanf("%99s", a);
            decode_encoded(a);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_encoded(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    return 0;
}