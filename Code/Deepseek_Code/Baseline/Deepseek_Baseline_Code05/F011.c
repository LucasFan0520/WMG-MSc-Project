/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct draft {
    char *title;
    char *body;
    struct draft *next;
} Draft;

Draft *drafts = NULL;
Draft *undo_stack = NULL;  // most recent discarded draft on top

Draft *find_draft(const char *title) {
    Draft *d = drafts;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = strdup(body);
        return;
    }
    Draft *new = malloc(sizeof(Draft));
    new->title = strdup(title);
    new->body = strdup(body);
    new->next = drafts;
    drafts = new;
}

void revise_draft(const char *title, const char *body) {
    create_draft(title, body); // overwrite
}

void discard_draft(const char *title) {
    Draft *d = drafts, *prev = NULL;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else drafts = d->next;
            // push to undo stack
            Draft *u = malloc(sizeof(Draft));
            u->title = d->title;
            u->body = d->body;
            u->next = undo_stack;
            undo_stack = u;
            free(d);
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore() {
    if (!undo_stack) return;
    Draft *u = undo_stack;
    undo_stack = u->next;
    // add back to drafts (as new)
    Draft *new = malloc(sizeof(Draft));
    new->title = u->title;
    new->body = u->body;
    new->next = drafts;
    drafts = new;
    free(u);
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) { printf("NOT_FOUND\n"); return; }
    for (char *c = d->body; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void print_all() {
    Draft *d = drafts;
    while (d) {
        printf("%s ", d->title);
        for (char *c = d->body; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        d = d->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], title[256], body[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "RESTORE") == 0) { restore(); continue; }
        if (strcmp(cmd, "PRINT") == 0) { print_all(); continue; }
        if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) find_draft_print(title);
            continue;
        }
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0 || strcmp(cmd, "DISCARD") == 0) {
            if (sscanf(line, "%*s %255s", title) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(title) + 1;
            if (*rest == ' ') rest++;
            if (strcmp(cmd, "CREATE") == 0) create_draft(title, rest);
            else if (strcmp(cmd, "REVISE") == 0) revise_draft(title, rest);
            else discard_draft(title);
        }
    }
    return 0;
}