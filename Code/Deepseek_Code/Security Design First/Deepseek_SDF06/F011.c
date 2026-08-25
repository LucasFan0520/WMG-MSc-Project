// F011.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

static Draft *drafts = NULL;
static size_t count = 0;
static size_t capacity = 0;

static Draft last_discarded = {NULL, NULL};

static ssize_t find_draft(const char *title) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(drafts[i].title, title) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_draft(const char *title, const char *body) {
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Draft *new = realloc(drafts, capacity * sizeof(Draft));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        drafts = new;
    }
    drafts[count].title = strdup(title);
    drafts[count].body = strdup(body);
    if (!drafts[count].title || !drafts[count].body) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void revise_draft(const char *title, const char *body) {
    ssize_t idx = find_draft(title);
    if (idx == -1) return;
    free(drafts[idx].body);
    drafts[idx].body = strdup(body);
    if (!drafts[idx].body) { fprintf(stderr, "Memory error\n"); exit(1); }
}

static void discard_draft(const char *title) {
    ssize_t idx = find_draft(title);
    if (idx == -1) return;
    // save discarded
    if (last_discarded.title) free(last_discarded.title);
    if (last_discarded.body) free(last_discarded.body);
    last_discarded.title = strdup(drafts[idx].title);
    last_discarded.body = strdup(drafts[idx].body);
    if (!last_discarded.title || !last_discarded.body) { fprintf(stderr, "Memory error\n"); exit(1); }
    // remove
    free(drafts[idx].title);
    free(drafts[idx].body);
    for (size_t i = idx; i < count - 1; i++)
        drafts[i] = drafts[i + 1];
    count--;
}

static void restore(void) {
    if (!last_discarded.title) return;
    add_draft(last_discarded.title, last_discarded.body);
    free(last_discarded.title);
    free(last_discarded.body);
    last_discarded.title = NULL;
    last_discarded.body = NULL;
}

static void find_draft_print(const char *title) {
    ssize_t idx = find_draft(title);
    if (idx == -1) return;
    printf("%s\n", drafts[idx].body);
}

static void print_all(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", drafts[i].title, drafts[i].body);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    free(last_discarded.title);
    free(last_discarded.body);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], title[256], body[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "CREATE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, body) == 2)
                add_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, body) == 2)
                revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore();
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }

    free(line);
    free_all();
    return 0;
}