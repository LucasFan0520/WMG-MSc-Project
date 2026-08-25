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
Draft *last_discarded = NULL; // only one

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
    d->next = drafts;
    drafts = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = strdup(body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL, *cur = drafts;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else drafts = cur->next;
            // save as last discarded
            if (last_discarded) {
                free(last_discarded->title);
                free(last_discarded->body);
                free(last_discarded);
            }
            last_discarded = cur; // transfer ownership (cur is unlinked)
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void restore_draft(void) {
    if (!last_discarded) return;
    // add back to drafts (at front)
    last_discarded->next = drafts;
    drafts = last_discarded;
    last_discarded = NULL;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    char *s = d->body;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void print_drafts(void) {
    Draft *d = drafts;
    while (d) {
        printf("%s: ", d->title);
        char *s = d->body;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
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
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *body = space + 1;
            while (*body == ' ') body++;
            create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *body = space + 1;
            while (*body == ' ') body++;
            revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    free_all();
    return 0;
}