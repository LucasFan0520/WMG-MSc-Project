// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *head = NULL;
Draft *trash = NULL;

Draft *find_draft(char *title) {
    for (Draft *d = head; d; d = d->next) {
        if (strcmp(d->title, title) == 0) return d;
    }
    return NULL;
}

void create_draft(char *title, char *body) {
    Draft *d = malloc(sizeof(Draft));
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = head;
    head = d;
}

void revise_draft(char *title, char *body) {
    Draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = strdup(body);
    }
}

void discard_draft(char *title) {
    Draft *prev = NULL;
    Draft *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            curr->next = trash;
            trash = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void restore_draft() {
    if (trash) {
        Draft *d = trash;
        trash = trash->next;
        d->next = head;
        head = d;
    }
}

void find_draft_body(char *title) {
    Draft *d = find_draft(title);
    if (d) printf("%s\n", d->body);
}

void print_drafts() {
    for (Draft *d = head; d; d = d->next) {
        printf("%s: %s\n", d->title, d->body);
    }
}

int main() {
    char cmd[20], title[100], body[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            scanf("%99s", title);
            fgets(body, sizeof(body), stdin);
            while (body[0] == ' ') memmove(body, body + 1, strlen(body));
            body[strcspn(body, "\n")] = 0;
            if (strcmp(cmd, "CREATE") == 0) create_draft(title, body);
            else revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            scanf("%99s", title);
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", title);
            find_draft_body(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    return 0;
}