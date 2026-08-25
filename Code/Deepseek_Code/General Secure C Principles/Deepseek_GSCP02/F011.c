// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

typedef struct Undo {
    char *title;
    char *body;
    int has;
} Undo;

Draft *d_head = NULL;
Undo undo = {NULL, NULL, 0};

Draft *find_draft(const char *title) {
    Draft *d = d_head;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *d = malloc(sizeof(Draft));
    if (!d) return;
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = d_head;
    d_head = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = strdup(body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL;
    Draft *d = d_head;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else d_head = d->next;
            if (undo.has) {
                free(undo.title);
                free(undo.body);
            }
            undo.title = strdup(d->title);
            undo.body = strdup(d->body);
            undo.has = 1;
            free(d->title);
            free(d->body);
            free(d);
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft(void) {
    if (!undo.has) return;
    if (find_draft(undo.title)) {
    }
    Draft *d = malloc(sizeof(Draft));
    if (!d) return;
    d->title = strdup(undo.title);
    d->body = strdup(undo.body);
    d->next = d_head;
    d_head = d;
    free(undo.title);
    free(undo.body);
    undo.has = 0;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    for (char *p = d->body; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void print_all_drafts(void) {
    Draft *d = d_head;
    while (d) {
        printf("%s ", d->title);
        for (char *p = d->body; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        d = d->next;
    }
}

void free_all(void) {
    while (d_head) {
        Draft *next = d_head->next;
        free(d_head->title);
        free(d_head->body);
        free(d_head);
        d_head = next;
    }
    if (undo.has) {
        free(undo.title);
        free(undo.body);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "CREATE") == 0) {
            char title[256], body[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", title, body) == 2) {
                create_draft(title, body);
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[256], body[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", title, body) == 2) {
                revise_draft(title, body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                discard_draft(title);
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                find_draft_print(title);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_drafts();
        }
    }

    free(line);
    free_all();
    return 0;
}