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

void create_draft(const char *t, const char *b) {
    Draft *d = malloc(sizeof(Draft));
    d->title = strdup(t);
    d->body = strdup(b);
    d->next = head;
    head = d;
}

void revise_draft(const char *t, const char *b) {
    for (Draft *d = head; d; d = d->next) {
        if (strcmp(d->title, t) == 0) {
            free(d->body);
            d->body = strdup(b);
            return;
        }
    }
}

void discard_draft(const char *t) {
    Draft *p = head;
    Draft *prev = NULL;
    while (p) {
        if (strcmp(p->title, t) == 0) {
            Draft *next = p->next;
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            p->next = trash;
            trash = p;
            return;
        }
        prev = p;
        p = p->next;
    }
}

void restore_draft() {
    if (!trash) {
        return;
    }
    Draft *d = trash;
    trash = trash->next;
    d->next = head;
    head = d;
}

void find_draft(const char *t) {
    for (Draft *d = head; d; d = d->next) {
        if (strcmp(d->title, t) == 0) {
            printf("%s %s\n", d->title, d->body);
            return;
        }
    }
}

void print_drafts() {
    for (Draft *d = head; d; d = d->next) {
        printf("%s %s\n", d->title, d->body);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char t[1000];
        char b[8000];
        b[0] = 0;
        sscanf(line, "%s %s %[^\n]", cmd, t, b);
        if (strcmp(cmd, "CREATE") == 0) {
            create_draft(t, b);
        } else if (strcmp(cmd, "REVISE") == 0) {
            revise_draft(t, b);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            discard_draft(t);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            find_draft(t);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    while (head) {
        Draft *n = head->next;
        free(head->title);
        free(head->body);
        free(head);
        head = n;
    }
    while (trash) {
        Draft *n = trash->next;
        free(trash->title);
        free(trash->body);
        free(trash);
        trash = n;
    }
    return 0;
}