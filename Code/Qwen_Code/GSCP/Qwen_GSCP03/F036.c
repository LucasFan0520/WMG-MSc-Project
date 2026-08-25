// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *name;
    char *data;
    struct Rec *next;
} Rec;

Rec *head = NULL;

void free_list() {
    while (head) {
        Rec *r = head; head = head->next;
        free(r->name); free(r->data); free(r);
    }
}

void print_underscores(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p = 0; p++;
            while (*p == ' ') p++;
            char *colon = strchr(p, ':');
            if (!colon) continue;
            *colon = 0;
            int len = atoi(p);
            char *data = colon + 1;
            if (len < 0 || (int)strlen(data) != len) continue;
            Rec *r = malloc(sizeof(Rec));
            r->name = strdup(name_start);
            r->data = strdup(data);
            r->next = head; head = r;
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Rec *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Rec *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->data); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            for (Rec *r = head; r; r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    print_underscores(r->data);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s\n", r->name);
            }
        }
    }
    free_list();
    return 0;
}