// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Para {
    char *id;
    char *text;
    struct Para *next;
} Para;

Para *head = NULL, *tail = NULL;

void free_list() {
    while (head) {
        Para *tmp = head;
        head = head->next;
        free(tmp->id);
        free(tmp->text);
        free(tmp);
    }
}

Para *find(const char *id) {
    for (Para *p = head; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

void print_spaces(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], id[256], text[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", id, text) < 1) continue;
            if (find(id)) continue;
            Para *p = malloc(sizeof(Para));
            p->id = strdup(id);
            p->text = strdup(text);
            p->next = NULL;
            if (tail) tail->next = p; else head = p;
            tail = p;
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", id, text) < 1) continue;
            Para *p = find(id);
            if (p) {
                char *nt = malloc(strlen(p->text) + strlen(text) + 2);
                sprintf(nt, "%s %s", p->text, text);
                free(p->text);
                p->text = nt;
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", id, text) < 1) continue;
            Para *p = find(id);
            if (p) {
                free(p->text);
                p->text = strdup(text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) != 1) continue;
            Para *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->id, id) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    if (curr == tail) tail = prev;
                    free(curr->id); free(curr->text); free(curr);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", id) != 1) continue;
            Para *p = find(id);
            if (p) print_spaces(p->text); else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Para *p = head; p; p = p->next) printf("%s\n", p->id);
        }
    }
    free_list();
    return 0;
}