// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct draft {
    char *title;
    char *body;
    struct draft *next;
} Draft;

Draft *drafts = NULL;
Draft *discarded = NULL; // only one

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

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
        if (!d->body) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    Draft *new = malloc(sizeof(Draft));
    if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
    new->title = strdup(title);
    new->body = strdup(body);
    if (!new->title || !new->body) { fprintf(stderr, "Memory error\n"); exit(1); }
    new->next = drafts;
    drafts = new;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = strdup(body);
    if (!d->body) { fprintf(stderr, "Memory error\n"); exit(1); }
}

void discard_draft(const char *title) {
    Draft *d = drafts, *prev = NULL;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else drafts = d->next;
            if (discarded) {
                free(discarded->title);
                free(discarded->body);
                free(discarded);
            }
            discarded = d;
            discarded->next = NULL;
            return;
        }
        prev = d;
        d = d->next;
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
    if (!d) return;
    for (char *s = d->body; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void print_drafts(void) {
    Draft *d = drafts;
    while (d) {
        printf("%s ", d->title);
        for (char *s = d->body; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
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
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "RESTORE") == 0) { restore_draft(); continue; }
        if (strcmp(cmd, "PRINT") == 0) { print_drafts(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0 || strcmp(cmd, "FIND") == 0) {
            char *title = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            if (strcmp(cmd, "CREATE") == 0) create_draft(title, rest);
            else if (strcmp(cmd, "REVISE") == 0) revise_draft(title, rest);
            else find_draft_print(title);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            discard_draft(rest);
        }
    }
    free_all();
    return 0;
}