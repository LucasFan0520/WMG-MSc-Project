// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *head = NULL, *tail = NULL;
Draft *discarded = NULL;

Draft *find_draft(const char *title) {
    Draft *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    Draft *d = malloc(sizeof(Draft));
    d->title = malloc(strlen(title) + 1);
    strcpy(d->title, title);
    d->body = malloc(strlen(body) + 1);
    strcpy(d->body, body);
    d->next = NULL;
    if (tail) {
        tail->next = d;
        tail = d;
    } else {
        head = tail = d;
    }
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = malloc(strlen(body) + 1);
    strcpy(d->body, body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            if (discarded) {
                free(discarded->title);
                free(discarded->body);
                free(discarded);
            }
            discarded = curr;
            curr->next = NULL;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void restore_draft(void) {
    if (!discarded) return;
    if (tail) {
        tail->next = discarded;
        tail = discarded;
    } else {
        head = tail = discarded;
    }
    discarded = NULL;
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (title && body) create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (title && body) revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = strtok(NULL, " ");
            if (title) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    free(line);
    free_all();
    return 0;
}