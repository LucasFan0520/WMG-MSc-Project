// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Enc {
    char *name;
    char *encoded;
    struct Enc *next;
} Enc;

Enc *head = NULL;

int main() {
    char cmd[20], name[256], encoded[2048];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "STORE") == 0) {
            scanf("%s %s", name, encoded);
            Enc *e = malloc(sizeof(Enc));
            e->name = strdup(name); e->encoded = strdup(encoded); e->next = head; head = e;
        } else if (strcmp(cmd, "DECODE") == 0) {
            scanf("%s", name);
            for (Enc *e = head; e; e = e->next) {
                if (strcmp(e->name, name) == 0) {
                    char *p = e->encoded;
                    int valid = 1;
                    while (*p) {
                        if (!isdigit(*p)) { valid = 0; break; }
                        long count = strtol(p, &p, 10);
                        if (count > 1000000) { valid = 0; break; }
                        if (!*p) { valid = 0; break; }
                        char c = *p++;
                        for (long i = 0; i < count; i++) printf("%c", c == ' ' ? '_' : c);
                    }
                    if (valid) printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            Enc *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->name); free(curr->encoded); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Enc *e = head; e; e = e->next) printf("%s\n", e->name);
        }
    }
    return 0;
}