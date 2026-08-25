/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct draft {
    char *title;
    char *body;
    struct draft *next;
} draft_t;

draft_t *head = NULL;
draft_t *tail = NULL;

// trash for most recently discarded
draft_t *trash = NULL;

draft_t *find_draft(const char *title) {
    draft_t *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    draft_t *d = malloc(sizeof(draft_t));
    if (!d) return;
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = NULL;
    if (!d->title || !d->body) {
        free(d->title); free(d->body); free(d);
        return;
    }
    if (tail) tail->next = d;
    else head = d;
    tail = d;
}

void revise_draft(const char *title, const char *body) {
    draft_t *d = find_draft(title);
    if (!d) return;
    char *new = strdup(body);
    if (!new) return;
    free(d->body);
    d->body = new;
}

void discard_draft(const char *title) {
    draft_t *prev = NULL, *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else head = d->next;
            if (d == tail) tail = prev;
            // store in trash, free old trash first
            if (trash) {
                free(trash->title);
                free(trash->body);
                free(trash);
            }
            trash = d;
            d->next = NULL;
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft(void) {
    if (!trash) return;
    if (find_draft(trash->title)) {
        // title exists, cannot restore
        return;
    }
    // move trash to list
    if (tail) tail->next = trash;
    else head = trash;
    tail = trash;
    trash = NULL;
}

void find_draft_print(const char *title) {
    draft_t *d = find_draft(title);
    if (!d) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = d->body; *s; s++) {
        if (*s == ' ') putchar('_');
        else putchar(*s);
    }
    putchar('\n');
}

void print_drafts(void) {
    draft_t *d = head;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

void free_all(void) {
    draft_t *d = head;
    while (d) {
        draft_t *nxt = d->next;
        free(d->title);
        free(d->body);
        free(d);
        d = nxt;
    }
    if (trash) {
        free(trash->title);
        free(trash->body);
        free(trash);
        trash = NULL;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], title[256];
        if (sscanf(line, "%9s %255s", cmd, title) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "CREATE") == 0) {
                const char *body = rest + strlen(title) + 1;
                create_draft(title, body);
            } else if (strcmp(cmd, "REVISE") == 0) {
                const char *body = rest + strlen(title) + 1;
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
    }
    free(line);
    free_all();
    return 0;
}