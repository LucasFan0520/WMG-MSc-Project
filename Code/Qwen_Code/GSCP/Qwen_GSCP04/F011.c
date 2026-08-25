// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *drafts = NULL;
Draft *discarded = NULL;

Draft *find_draft(const char *title) {
    Draft *curr = drafts;
    while (curr) {
        if (strcmp(curr->title, title) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = strdup(body);
    } else {
        d = malloc(sizeof(Draft));
        d->title = strdup(title);
        d->body = strdup(body);
        d->next = drafts;
        drafts = d;
    }
}

void discard_draft(const char *title) {
    Draft *curr = drafts, *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else drafts = curr->next;
            curr->next = discarded;
            discarded = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void restore_draft() {
    if (!discarded) return;
    Draft *d = discarded;
    discarded = discarded->next;
    d->next = drafts;
    drafts = d;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (d) printf("%s %s\n", d->title, d->body);
}

void print_drafts() {
    Draft *curr = drafts;
    while (curr) {
        printf("%s %s\n", curr->title, curr->body);
        curr = curr->next;
    }
}

void free_all() {
    Draft *curr = drafts;
    while (curr) {
        Draft *next = curr->next;
        free(curr->title);
        free(curr->body);
        free(curr);
        curr = next;
    }
    curr = discarded;
    while (curr) {
        Draft *next = curr->next;
        free(curr->title);
        free(curr->body);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], title[256], body[4096] = {0};
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, title, body);
        
        if (strcmp(cmd, "CREATE") == 0 && n >= 2) create_draft(title, body);
        else if (strcmp(cmd, "REVISE") == 0 && n >= 2) create_draft(title, body);
        else if (strcmp(cmd, "DISCARD") == 0 && n >= 1) discard_draft(title);
        else if (strcmp(cmd, "RESTORE") == 0) restore_draft();
        else if (strcmp(cmd, "FIND") == 0 && n >= 1) find_draft_print(title);
        else if (strcmp(cmd, "PRINT") == 0) print_drafts();
    }
    free_all();
    return 0;
}