/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *head = NULL;
Draft *last_discarded = NULL;

Draft* find_draft(const char *title) {
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
    d->next = head;
    head = d;
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
            if (last_discarded) {
                free(last_discarded->title);
                free(last_discarded->body);
                free(last_discarded);
            }
            last_discarded = d; // store whole draft
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft() {
    if (!last_discarded) return;
    Draft *d = malloc(sizeof(Draft));
    d->title = strdup(last_discarded->title);
    d->body = strdup(last_discarded->body);
    d->next = head;
    head = d;
    free(last_discarded->title);
    free(last_discarded->body);
    free(last_discarded);
    last_discarded = NULL;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    printf("%s %s\n", d->title, d->body);
}

void print_drafts() {
    Draft *d = head;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

void free_all() {
    while (head) {
        Draft *tmp = head;
        head = head->next;
        free(tmp->title);
        free(tmp->body);
        free(tmp);
    }
    if (last_discarded) {
        free(last_discarded->title);
        free(last_discarded->body);
        free(last_discarded);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char title[256]; char body[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, title, body);
        if (strcmp(cmd, "CREATE") == 0 && n == 3) {
            create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0 && n == 3) {
            revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0 && n == 2) {
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0 && n == 1) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_drafts();
        }
    }
    free_all();
    return 0;
}