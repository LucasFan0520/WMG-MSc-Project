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
Draft *tail = NULL;
Draft *discarded = NULL; // most recently discarded

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
    d->title = malloc(strlen(title)+1);
    strcpy(d->title, title);
    d->body = malloc(strlen(body)+1);
    strcpy(d->body, body);
    d->next = NULL;
    if (tail) tail->next = d;
    else head = d;
    tail = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = malloc(strlen(body)+1);
    strcpy(d->body, body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL;
    Draft *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else head = d->next;
            if (d == tail) tail = prev;
            if (discarded) {
                free(discarded->title);
                free(discarded->body);
                free(discarded);
            }
            discarded = d;
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft(void) {
    if (!discarded) return;
    if (find_draft(discarded->title)) {
        // cannot restore, title exists, do nothing (per spec? we can ignore)
        // but we keep discarded? spec says restore if available; if exists, maybe ignore?
        // We'll ignore.
        return;
    }
    Draft *d = malloc(sizeof(Draft));
    d->title = malloc(strlen(discarded->title)+1);
    strcpy(d->title, discarded->title);
    d->body = malloc(strlen(discarded->body)+1);
    strcpy(d->body, discarded->body);
    d->next = NULL;
    if (tail) tail->next = d;
    else head = d;
    tail = d;
    free(discarded->title);
    free(discarded->body);
    free(discarded);
    discarded = NULL;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    printf("%s\n", d->body);
}

void print_drafts(void) {
    Draft *d = head;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

void free_all(void) {
    Draft *d = head;
    while (d) {
        Draft *next = d->next;
        free(d->title);
        free(d->body);
        free(d);
        d = next;
    }
    if (discarded) {
        free(discarded->title);
        free(discarded->body);
        free(discarded);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = strtok(rest, " ");
            if (!title) continue;
            char *body = strtok(NULL, "");
            if (!body) body = "";
            create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = strtok(rest, " ");
            if (!title) continue;
            char *body = strtok(NULL, "");
            if (!body) body = "";
            revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = strtok(rest, " ");
            if (title) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(rest, " ");
            if (title) find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    free_all();
    return 0;
}