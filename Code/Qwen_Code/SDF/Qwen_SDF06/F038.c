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

void store_encoded(const char *name, const char *data) {
    Encoded *e = malloc(sizeof(Encoded));
    if (!e) return;
    e->name = strdup(name);
    e->data = strdup(data);
    if (!e->name || !e->data) { free(e->name); free(e->data); free(e); return; }
    e->next = head;
    head = e;
}

void decode(const char *name) {
    for (Encoded *e = head; e; e = e->next) {
        if (strcmp(e->name, name) == 0) {
            char *p = e->data;
            while (*p) {
                if (!isdigit(*p)) return;
                long count = strtol(p, &p, 10);
                if (count < 0 || count > 1000000) return;
                if (!*p) return;
                char c = *p++;
                for (long i = 0; i < count; i++) {
                    if (c == ' ') putchar('_');
                    else putchar(c);
                }
            }
            putchar('\n');
            return;
        }
    }
}

void delete_encoded(const char *name) {
    Encoded *curr = head;
    Encoded *prev = NULL;
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

void free_all() {
    Encoded *e = head;
    while (e) {
        Encoded *next = e->next;
        free(e->name);
        free(e->data);
        free(e);
        e = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "STORE") == 0 && n >= 3) {
            store_encoded(arg1, arg2);
        } else if (strcmp(cmd, "DECODE") == 0 && n >= 2) {
            decode(arg1);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_encoded(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    free_all();
    return 0;
}