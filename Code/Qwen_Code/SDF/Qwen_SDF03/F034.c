// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
} Note;

Note *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

void clear_notes() {
    while (head) {
        Note *next = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = next;
    }
}

int main(int argc, char **argv) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                FILE *f = fopen(argv[1], "r");
                if (f) {
                    Note *new_head = NULL;
                    int valid = 1;
                    while (fgets(line, sizeof(line), f)) {
                        line[strcspn(line, "\n")] = '\0';
                        char *p = line;
                        char *name = get_token(&p);
                        if (!name) {
                            valid = 0;
                            break;
                        }
                        Note *n = malloc(sizeof(Note));
                        if (!n) {
                            valid = 0;
                            break;
                        }
                        n->name = strdup(name);
                        n->note = strdup(p);
                        n->next = new_head;
                        new_head = n;
                    }
                    fclose(f);
                    if (valid) {
                        clear_notes();
                        head = new_head;
                    } else {
                        while (new_head) {
                            Note *next = new_head->next;
                            free(new_head->name);
                            free(new_head->note);
                            free(new_head);
                            new_head = next;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Note *n = malloc(sizeof(Note));
                if (n) {
                    n->name = strdup(name);
                    n->note = strdup(ptr);
                    n->next = head;
                    head = n;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Note *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free(curr->name);
                        free(curr->note);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Note *n = head; n; n = n->next) {
                printf("%s %s\n", n->name, n->note);
            }
        }
    }
    clear_notes();
    return 0;
}