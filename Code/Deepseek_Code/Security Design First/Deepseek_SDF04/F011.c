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
Draft *last_discarded = NULL;

Draft *find_draft(const char *title) {
    Draft *d = drafts;
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
    d->next = NULL;
    if (!d->title || !d->body) {
        free(d->title); free(d->body); free(d);
        return;
    }
    if (!drafts) drafts = d;
    else {
        Draft *last = drafts;
        while (last->next) last = last->next;
        last->next = d;
    }
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    char *newbody = strdup(body);
    if (!newbody) return;
    free(d->body);
    d->body = newbody;
}

void discard_draft(const char *title) {
    Draft *d = drafts, *prev = NULL;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else drafts = d->next;
            if (last_discarded) {
                free(last_discarded->title);
                free(last_discarded->body);
                free(last_discarded);
            }
            last_discarded = d;
            d->next = NULL;
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft(void) {
    if (!last_discarded) return;
    if (find_draft(last_discarded->title)) {
        // if title exists, free the restored one? skip
        return;
    }
    Draft *d = malloc(sizeof(Draft));
    if (!d) return;
    d->title = strdup(last_discarded->title);
    d->body = strdup(last_discarded->body);
    d->next = NULL;
    if (!d->title || !d->body) {
        free(d->title); free(d->body); free(d);
        return;
    }
    if (!drafts) drafts = d;
    else {
        Draft *last = drafts;
        while (last->next) last = last->next;
        last->next = d;
    }
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    printf("%s\n", d->body);
}

void print_all_drafts(void) {
    Draft *d = drafts;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

void free_all(void) {
    Draft *d = drafts;
    while (d) {
        Draft *next = d->next;
        free(d->title);
        free(d->body);
        free(d);
        d = next;
    }
    if (last_discarded) {
        free(last_discarded->title);
        free(last_discarded->body);
        free(last_discarded);
        last_discarded = NULL;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], title[256], body[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, body) == 2)
                create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, body) == 2)
                revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_drafts();
        }
    }
    free(line);
    free_all();
    return 0;
}