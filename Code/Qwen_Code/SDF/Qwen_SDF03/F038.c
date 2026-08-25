// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 65536

typedef struct Enc {
    char *name;
    char *encoded;
    struct Enc *next;
} Enc;

Enc *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "STORE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Enc *e = malloc(sizeof(Enc));
                if (e) {
                    e->name = strdup(name);
                    e->encoded = strdup(ptr);
                    e->next = head;
                    head = e;
                }
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                for (Enc *e = head; e; e = e->next) {
                    if (strcmp(e->name, name) == 0) {
                        char *p = e->encoded;
                        while (*p) {
                            if (!isdigit((unsigned char)*p)) break;
                            long count = strtol(p, &p, 10);
                            if (count > 1000000 || !*p) break;
                            for (long i = 0; i < count; i++) {
                                putchar(*p == ' ' ? '_' : *p);
                            }
                            p++;
                        }
                        putchar('\n');
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Enc *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free(curr->name);
                        free(curr->encoded);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Enc *e = head; e; e = e->next) {
                printf("%s\n", e->name);
            }
        }
    }
    while (head) {
        Enc *next = head->next;
        free(head->name);
        free(head->encoded);
        free(head);
        head = next;
    }
    return 0;
}