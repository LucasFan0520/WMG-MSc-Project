// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft { char *title; char *body; struct Draft *next; } Draft;
Draft *drafts = NULL;
Draft *discarded = NULL; // only one

Draft *find_draft(const char *title) {
    for (Draft *d = drafts; d; d = d->next)
        if (strcmp(d->title, title) == 0) return d;
    return NULL;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *d = malloc(sizeof(Draft));
    if (!d) return;
    d->title = strdup(title); d->body = strdup(body);
    d->next = drafts; drafts = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body); d->body = strdup(body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL;
    for (Draft *d = drafts; d; prev = d, d = d->next) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next; else drafts = d->next;
            if (discarded) { free(discarded->title); free(discarded->body); free(discarded); }
            discarded = d;
            return;
        }
    }
}

void restore_draft(void) {
    if (!discarded) return;
    discarded->next = drafts;
    drafts = discarded;
    discarded = NULL;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", d->body);
}

void print_drafts(void) {
    for (Draft *d = drafts; d; d = d->next) printf("%s\n%s\n", d->title, d->body);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "CREATE") == 0) {
            char title[256], body[4096];
            if (sscanf(rest, "%255s", title) == 1) {
                char *b = rest + strlen(title);
                while (*b == ' ') b++;
                strcpy(body, b);
                create_draft(title, body);
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[256], body[4096];
            if (sscanf(rest, "%255s", title) == 1) {
                char *b = rest + strlen(title);
                while (*b == ' ') b++;
                strcpy(body, b);
                revise_draft(title, body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    while (drafts) { Draft *d = drafts; drafts = d->next; free(d->title); free(d->body); free(d); }
    if (discarded) { free(discarded->title); free(discarded->body); free(discarded); }
    return 0;
}