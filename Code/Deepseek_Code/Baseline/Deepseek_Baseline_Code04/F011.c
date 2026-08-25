/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *drafts = NULL;
Draft *last_discarded = NULL;  // used for undo

Draft *find_draft(const char *title) {
    Draft *d = drafts;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = strdup(body);
        return;
    }
    Draft *nd = malloc(sizeof(Draft));
    nd->title = strdup(title);
    nd->body = strdup(body);
    nd->next = drafts;
    drafts = nd;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = strdup(body);
    }
}

void discard_draft(const char *title) {
    Draft *prev = NULL, *cur = drafts;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else drafts = cur->next;
            // store for undo
            if (last_discarded) {
                free(last_discarded->title);
                free(last_discarded->body);
                free(last_discarded);
            }
            last_discarded = malloc(sizeof(Draft));
            last_discarded->title = cur->title;
            last_discarded->body = cur->body;
            last_discarded->next = NULL;
            free(cur); // but we keep title/body in last_discarded
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void restore_draft(void) {
    if (!last_discarded) return;
    Draft *d = malloc(sizeof(Draft));
    d->title = strdup(last_discarded->title);
    d->body = strdup(last_discarded->body);
    d->next = drafts;
    drafts = d;
    free(last_discarded->title);
    free(last_discarded->body);
    free(last_discarded);
    last_discarded = NULL;
}

void print_draft(Draft *d) {
    printf("%s\n%s\n", d->title, d->body);
}

void print_all(void) {
    Draft *d = drafts;
    while (d) {
        printf("%s\n", d->title);
        d = d->next;
    }
}

void free_drafts(void) {
    while (drafts) {
        Draft *tmp = drafts;
        drafts = drafts->next;
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char title[256];
        int n = sscanf(line, "%s %s", cmd, title);
        if (strcmp(cmd, "CREATE") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(title) + 1;
            while (*p == ' ') p++;
            if (*p) create_draft(title, p);
        } else if (strcmp(cmd, "REVISE") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(title) + 1;
            while (*p == ' ') p++;
            if (*p) revise_draft(title, p);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            if (n < 2) continue;
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            Draft *d = find_draft(title);
            if (d) print_draft(d);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_drafts();
    return 0;
}