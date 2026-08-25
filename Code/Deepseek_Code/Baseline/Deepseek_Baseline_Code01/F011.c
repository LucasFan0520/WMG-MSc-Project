// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct draft {
    char *title;
    char *body;
    struct draft *next;
} Draft;

Draft *head = NULL;
Draft *tail = NULL;
Draft *trash = NULL;

Draft *find_draft(const char *title) {
    Draft *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *d = malloc(sizeof(Draft));
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = NULL;
    if (tail) tail->next = d;
    else head = d;
    tail = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = strdup(body);
}

void discard_draft(const char *title) {
    Draft *d = head, *prev = NULL;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else head = d->next;
            if (d == tail) tail = prev;
            if (trash) { free(trash->title); free(trash->body); free(trash); }
            trash = d;
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft(void) {
    if (!trash) return;
    Draft *d = trash;
    trash = NULL;
    d->next = NULL;
    if (tail) tail->next = d;
    else head = d;
    tail = d;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    printf("%s %s\n", d->title, d->body);
}

void print_drafts(void) {
    Draft *d = head;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], body[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", title, body) == 2)
                create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", title, body) == 2)
                revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    return 0;
}