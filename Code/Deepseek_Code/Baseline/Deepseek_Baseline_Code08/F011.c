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
Draft *last_discarded = NULL;

Draft* find_draft(const char *title) {
    Draft *cur = head;
    while (cur) {
        if (strcmp(cur->title, title) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *new = malloc(sizeof(Draft));
    new->title = strdup(title);
    new->body = strdup(body);
    new->next = NULL;
    if (!head) head = new;
    else {
        Draft *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = strdup(body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (last_discarded) {
                free(last_discarded->title);
                free(last_discarded->body);
                free(last_discarded);
            }
            last_discarded = malloc(sizeof(Draft));
            last_discarded->title = strdup(cur->title);
            last_discarded->body = strdup(cur->body);
            last_discarded->next = NULL;
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->title);
            free(cur->body);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void restore_draft() {
    if (!last_discarded) return;
    if (find_draft(last_discarded->title)) {
        // cannot restore if exists
        return;
    }
    Draft *new = malloc(sizeof(Draft));
    new->title = strdup(last_discarded->title);
    new->body = strdup(last_discarded->body);
    new->next = NULL;
    if (!head) head = new;
    else {
        Draft *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
    free(last_discarded->title);
    free(last_discarded->body);
    free(last_discarded);
    last_discarded = NULL;
}

void print_draft(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    printf("%s\n", d->body);
}

void print_all() {
    Draft *cur = head;
    while (cur) {
        printf("%s %s\n", cur->title, cur->body);
        cur = cur->next;
    }
}

void free_all() {
    Draft *cur = head;
    while (cur) {
        Draft *tmp = cur;
        cur = cur->next;
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *body = strtok(NULL, "");
            if (!body) body = "";
            create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *body = strtok(NULL, "");
            if (!body) body = "";
            revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = strtok(NULL, " ");
            if (title) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) print_draft(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}