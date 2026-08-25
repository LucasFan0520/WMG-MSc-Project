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
Draft *tail = NULL;
Draft *discarded = NULL; // most recently discarded

Draft *find_draft(char *title) {
    for (Draft *d = head; d; d = d->next)
        if (strcmp(d->title, title) == 0) return d;
    return NULL;
}

void create_draft(char *title, char *body) {
    Draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = strdup(body);
        return;
    }
    Draft *new = malloc(sizeof(Draft));
    new->title = strdup(title);
    new->body = strdup(body);
    new->next = NULL;
    if (!tail) head = tail = new;
    else { tail->next = new; tail = new; }
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
    for (Draft *d = head; d; d = d->next) {
        if (strcmp(d->title, title) == 0) {
            if (discarded) {
                free(discarded->title);
                free(discarded->body);
                free(discarded);
            }
            discarded = malloc(sizeof(Draft));
            discarded->title = strdup(d->title);
            discarded->body = strdup(d->body);
            discarded->next = NULL;
            // remove from list
            if (prev) prev->next = d->next;
            else head = d->next;
            if (d == tail) tail = prev;
            free(d->title);
            free(d->body);
            free(d);
            return;
        }
        prev = d;
    }
}

void restore_draft(void) {
    if (!discarded) return;
    Draft *new = malloc(sizeof(Draft));
    new->title = strdup(discarded->title);
    new->body = strdup(discarded->body);
    new->next = NULL;
    if (!tail) head = tail = new;
    else { tail->next = new; tail = new; }
    free(discarded->title);
    free(discarded->body);
    free(discarded);
    discarded = NULL;
}

void find_draft_print(char *title) {
    Draft *d = find_draft(title);
    if (d) printf("%s\n", d->body);
}

void print_all(void) {
    for (Draft *d = head; d; d = d->next)
        printf("%s %s\n", d->title, d->body);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "CREATE") == 0 && n == 3) {
            create_draft(arg1, arg2);
        } else if (strcmp(cmd, "REVISE") == 0 && n == 3) {
            revise_draft(arg1, arg2);
        } else if (strcmp(cmd, "DISCARD") == 0 && n == 2) {
            discard_draft(arg1);
        } else if (strcmp(cmd, "RESTORE") == 0 && n == 1) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_draft_print(arg1);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_all();
        }
    }
    free(line);
    free_all();
    return 0;
}