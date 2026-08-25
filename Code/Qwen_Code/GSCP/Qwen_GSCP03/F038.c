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

void free_list() {
    while (head) {
        Enc *e = head; head = head->next;
        free(e->name); free(e->encoded); free(e);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], encoded[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "STORE") == 0) {
            if (sscanf(line, "%*s %255s %4095s", name, encoded) != 2) continue;
            Enc *e = malloc(sizeof(Enc));
            e->name = strdup(name); e->encoded = strdup(encoded);
            e->next = head; head = e;
        } else if (strcmp(cmd, "DECODE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            for (Enc *e = head; e; e = e->next) {
                if (strcmp(e->name, name) == 0) {
                    char *p = e->encoded;
                    while (*p) {
                        if (!isdigit(*p)) break;
                        long count = strtol(p, &p, 10);
                        if (count <= 0 || count > 1000000 || !*p) {
                            count = -1; break;
                        }
                        char c = *p++;
                        for (long i = 0; i < count; i++) {
                            printf("%c", c == ' ' ? '_' : c);
                        }
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Enc *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Enc *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->encoded); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Enc *e = head; e; e = e->next) {
                printf("%s\n", e->name);
            }
        }
    }
    free_list();
    return 0;
}