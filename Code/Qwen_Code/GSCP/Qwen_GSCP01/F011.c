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
Draft *trash_stack = NULL;

Draft *find_draft(const char *title) {
    Draft *curr = drafts;
    while (curr) {
        if (strcmp(curr->title, title) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *d = malloc(sizeof(Draft));
    if (!d) exit(EXIT_FAILURE);
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = drafts;
    drafts = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = strdup(body);
}

void discard_draft(const char *title) {
    Draft *curr = drafts;
    Draft *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else drafts = curr->next;
            curr->next = trash_stack;
            trash_stack = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void restore_draft() {
    if (!trash_stack) return;
    Draft *d = trash_stack;
    trash_stack = trash_stack->next;
    d->next = drafts;
    drafts = d;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    printf("%s %s\n", d->title, d->body);
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
    curr = trash_stack;
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
        
        char cmd[32] = {0};
        char title[256] = {0};
        char body[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, title, body);
        
        if (strcmp(cmd, "CREATE") == 0 && matched == 3) {
            create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0 && matched == 3) {
            revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    free_all();
    return 0;
}