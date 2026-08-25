/*
 * F011.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct draft {
    char *title;
    char *body;
    struct draft *next;
} Draft;

Draft *head = NULL, *tail = NULL;
char *undo_title = NULL, *undo_body = NULL;

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
    Draft *prev = NULL, *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else head = d->next;
            if (d == tail) tail = prev;
            free(undo_title);
            free(undo_body);
            undo_title = d->title;
            undo_body = d->body;
            free(d);
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft(void) {
    if (!undo_title) return;
    create_draft(undo_title, undo_body);
    free(undo_title);
    free(undo_body);
    undo_title = undo_body = NULL;
}

void find_draft_cmd(const char *title) {
    Draft *d = find_draft(title);
    if (!d) {
        printf("NOT_FOUND\n");
        return;
    }
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
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256];
        if (sscanf(line, "%31s %255s", cmd, title) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(title) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "CREATE") == 0) {
            create_draft(title, rest);
        } else if (strcmp(cmd, "REVISE") == 0) {
            revise_draft(title, rest);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            find_draft_cmd(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    return 0;
}