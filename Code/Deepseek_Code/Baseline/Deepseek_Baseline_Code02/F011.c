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
Draft *discarded = NULL; // only one most recent discarded

Draft *find_draft(const char *title) {
    Draft *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *d = malloc(sizeof(Draft));
    d->title = malloc(strlen(title) + 1);
    strcpy(d->title, title);
    d->body = malloc(strlen(body) + 1);
    strcpy(d->body, body);
    d->next = head;
    head = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = malloc(strlen(body) + 1);
    strcpy(d->body, body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL;
    Draft *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else head = d->next;
            if (discarded) {
                free(discarded->title);
                free(discarded->body);
                free(discarded);
            }
            discarded = d;
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft(void) {
    if (!discarded) return;
    if (find_draft(discarded->title)) {
        // if title exists, cannot restore? The spec says restore if available, but if duplicate? we'll ignore.
        return;
    }
    Draft *d = malloc(sizeof(Draft));
    d->title = malloc(strlen(discarded->title) + 1);
    strcpy(d->title, discarded->title);
    d->body = malloc(strlen(discarded->body) + 1);
    strcpy(d->body, discarded->body);
    d->next = head;
    head = d;
    free(discarded->title);
    free(discarded->body);
    free(discarded);
    discarded = NULL;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    for (char *c = d->body; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void print_drafts(void) {
    Draft *d = head;
    while (d) {
        printf("%s:", d->title);
        for (char *c = d->body; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        d = d->next;
    }
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            create_draft(title, p);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            revise_draft(title, p);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            discard_draft(p);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_draft_print(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    free_all();
    return 0;
}