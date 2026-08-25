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
Draft *discarded = NULL;

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
    Draft *d = drafts, *prev = NULL;
    while (d && strcmp(d->title, title) != 0) {
        prev = d;
        d = d->next;
    }
    if (!d) return;
    if (prev) prev->next = d->next;
    else drafts = d->next;
    if (discarded) {
        free(discarded->title);
        free(discarded->body);
        free(discarded);
    }
    discarded = d;
}

void restore_draft(void) {
    if (!discarded) return;
    Draft *d = discarded;
    discarded = NULL;
    if (find_draft(d->title)) {
        free(d->title);
        free(d->body);
        free(d);
        return;
    }
    d->next = drafts;
    drafts = d;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) { printf("NOT_FOUND\n"); return; }
    printf("%s ", d->title);
    for (char *c = d->body; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void print_drafts(void) {
    Draft *d = drafts;
    while (d) {
        printf("%s ", d->title);
        for (char *c = d->body; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
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
    if (discarded) {
        free(discarded->title);
        free(discarded->body);
        free(discarded);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char title[256];
            char *p = line + 7;
            if (sscanf(p, "%255s", title) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            create_draft(title, p);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[256];
            char *p = line + 7;
            if (sscanf(p, "%255s", title) != 1) continue;
            while (*p && *p != ' ') p++;
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
    free(line);
    return 0;
}