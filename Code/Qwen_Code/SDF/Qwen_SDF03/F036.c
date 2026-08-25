// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Rec {
    char *name;
    char *data;
    struct Rec *next;
} Rec;

Rec *head = NULL;

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
        if (strcmp(cmd, "ADD") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                char *colon = strchr(ptr, ':');
                if (colon) {
                    *colon = '\0';
                    long len = strtol(ptr, NULL, 10);
                    char *data = colon + 1;
                    if (len >= 0 && (size_t)len == strlen(data)) {
                        Rec *r = malloc(sizeof(Rec));
                        if (r) {
                            r->name = strdup(name);
                            r->data = strdup(data);
                            r->next = head;
                            head = r;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Rec *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free(curr->name);
                        free(curr->data);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                for (Rec *r = head; r; r = r->next) {
                    if (strcmp(r->name, name) == 0) {
                        for (char *p = r->data; *p; p++) {
                            putchar(*p == ' ' ? '_' : *p);
                        }
                        putchar('\n');
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s\n", r->name);
            }
        }
    }
    while (head) {
        Rec *next = head->next;
        free(head->name);
        free(head->data);
        free(head);
        head = next;
    }
    return 0;
}