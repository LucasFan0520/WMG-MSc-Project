// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft { char *title, *body; struct Draft *next; } Draft;
Draft *head = NULL, *discarded = NULL;

void create_draft(const char *title, const char *body) {
    for (Draft *d = head; d; d = d->next) {
        if (strcmp(d->title, title) == 0) {
            free(d->body);
            d->body = strdup(body);
            return;
        }
    }
    Draft *d = malloc(sizeof(Draft));
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = head;
    head = d;
}

void revise_draft(const char *title, const char *body) { create_draft(title, body); }

void discard_draft(const char *title) {
    Draft **p = &head;
    while (*p) {
        if (strcmp((*p)->title, title) == 0) {
            Draft *tmp = *p;
            *p = tmp->next;
            tmp->next = discarded;
            discarded = tmp;
            return;
        }
        p = &(*p)->next;
    }
}

void restore_draft() {
    if (discarded) {
        Draft *tmp = discarded;
        discarded = discarded->next;
        tmp->next = head;
        head = tmp;
    }
}

void find_draft(const char *title) {
    for (Draft *d = head; d; d = d->next) {
        if (strcmp(d->title, title) == 0) { printf("%s\n", d->body); return; }
    }
}

void print_drafts() {
    for (Draft *d = head; d; d = d->next) printf("%s %s\n", d->title, d->body);
}

void cleanup() {
    while (head) {
        Draft *tmp = head->next;
        free(head->title);
        free(head->body);
        free(head);
        head = tmp;
    }
    while (discarded) {
        Draft *tmp = discarded->next;
        free(discarded->title);
        free(discarded->body);
        free(discarded);
        discarded = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256], body[8192] = "";
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) strcpy(body, t + 1); }
                create_draft(title, body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) restore_draft();
        else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) find_draft(title);
        } else if (strcmp(cmd, "PRINT") == 0) print_drafts();
    }
    cleanup();
    return 0;
}