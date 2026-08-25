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
Draft *last_discarded = NULL; // only one, last discarded

Draft* find_draft(const char *title) {
    Draft *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void free_draft(Draft *d) {
    free(d->title);
    free(d->body);
    free(d);
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *new = malloc(sizeof(Draft));
    new->title = strdup(title);
    new->body = strdup(body);
    new->next = NULL;
    if (!head) head = new;
    else {
        Draft *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
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
            // store discarded
            if (last_discarded) free_draft(last_discarded);
            last_discarded = d;
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore() {
    if (!last_discarded) return;
    // Append to end
    last_discarded->next = NULL;
    if (!head) head = last_discarded;
    else {
        Draft *last = head;
        while (last->next) last = last->next;
        last->next = last_discarded;
    }
    last_discarded = NULL;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    printf("%s %s\n", d->title, d->body);
}

void print_all() {
    Draft *d = head;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (title) {
                if (!body) body = "";
                create_draft(title, body);
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (title && body) revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = strtok(NULL, " ");
            if (title) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    while (head) {
        Draft *tmp = head;
        head = head->next;
        free_draft(tmp);
    }
    if (last_discarded) free_draft(last_discarded);
    return 0;
}