// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Rec {
    char *name;
    char *note;
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
        if (strcmp(cmd, "BATCH") == 0) {
            char *nstr = get_token(&ptr);
            if (nstr) {
                int n = atoi(nstr);
                Rec *batch = NULL;
                int valid = 1;
                for (int i = 0; i < n; i++) {
                    if (!fgets(line, sizeof(line), stdin)) {
                        valid = 0;
                        break;
                    }
                    line[strcspn(line, "\n")] = '\0';
                    char *p = line;
                    char *name = get_token(&p);
                    if (!name) {
                        valid = 0;
                        break;
                    }
                    Rec *r = malloc(sizeof(Rec));
                    if (!r) {
                        valid = 0;
                        break;
                    }
                    r->name = strdup(name);
                    r->note = strdup(p);
                    r->next = batch;
                    batch = r;
                }
                if (valid) {
                    while (batch) {
                        Rec *next = batch->next;
                        batch->next = head;
                        head = batch;
                        batch = next;
                    }
                } else {
                    while (batch) {
                        Rec *next = batch->next;
                        free(batch->name);
                        free(batch->note);
                        free(batch);
                        batch = next;
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
                        free(curr->note);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                for (Rec *r = head; r; r = r->next) {
                    if (strcmp(r->name, name) == 0) {
                        printf("%s\n", r->note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s %s\n", r->name, r->note);
            }
        }
    }
    while (head) {
        Rec *next = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = next;
    }
    return 0;
}