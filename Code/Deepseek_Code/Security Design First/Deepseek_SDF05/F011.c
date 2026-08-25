// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *drafts = NULL;
Draft *discarded = NULL;  // only one

Draft *find_draft(const char *title) {
    Draft *d = drafts;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *d = malloc(sizeof(Draft));
    if (!d) return;
    d->title = safe_strdup(title);
    d->body = safe_strdup(body);
    d->next = drafts;
    drafts = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = safe_strdup(body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL, *cur = drafts;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else drafts = cur->next;
            // store discarded
            if (discarded) {
                free(discarded->title);
                free(discarded->body);
                free(discarded);
            }
            discarded = cur;
            // do not free cur, we keep it
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void restore_draft(void) {
    if (!discarded) return;
    // add back to drafts
    discarded->next = drafts;
    drafts = discarded;
    discarded = NULL;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n%s\n", d->title, d->body);
}

void print_drafts(void) {
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
    if (discarded) {
        free(discarded->title);
        free(discarded->body);
        free(discarded);
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            if (n < 2) continue;
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            if (strcmp(cmd, "CREATE") == 0) create_draft(arg1, p);
            else revise_draft(arg1, p);
        } else if (strcmp(cmd, "DISCARD") == 0 && n >= 2) {
            discard_draft(arg1);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_draft_print(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    free_all();
    return 0;
}