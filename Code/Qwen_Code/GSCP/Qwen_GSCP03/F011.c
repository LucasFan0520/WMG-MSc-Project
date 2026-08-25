// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title, *body;
    struct Draft *next;
} Draft;

Draft *head = NULL, *discard_stack = NULL;

void free_list(Draft *d) {
    while (d) { Draft *t = d; d = d->next; free(t->title); free(t->body); free(t); }
}

Draft *find_draft(const char *title) {
    for (Draft *d = head; d; d = d->next)
        if (strcmp(d->title, title) == 0) return d;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], title[256], body[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "CREATE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, body) < 1) continue;
            if (find_draft(title)) continue;
            Draft *d = malloc(sizeof(Draft));
            d->title = strdup(title); d->body = strdup(body); d->next = head; head = d;
        } else if (strcmp(cmd, "REVISE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, body) < 1) continue;
            Draft *d = find_draft(title);
            if (d) { free(d->body); d->body = strdup(body); }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            if (sscanf(line, "%*s %255s", title) != 1) continue;
            Draft *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    Draft *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    tmp->next = discard_stack; discard_stack = tmp;
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (discard_stack) {
                Draft *d = discard_stack;
                discard_stack = discard_stack->next;
                d->next = head; head = d;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) != 1) continue;
            Draft *d = find_draft(title);
            if (d) printf("%s %s\n", d->title, d->body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Draft *d = head; d; d = d->next) printf("%s %s\n", d->title, d->body);
        }
    }
    free_list(head);
    free_list(discard_stack);
    return 0;
}