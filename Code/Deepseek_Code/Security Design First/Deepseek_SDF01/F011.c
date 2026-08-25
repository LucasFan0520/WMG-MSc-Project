// filename: F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

static Draft *drafts = NULL;
static Draft *discarded = NULL; // most recently discarded

static void cleanup(void) {
    while (drafts) {
        Draft *tmp = drafts;
        drafts = drafts->next;
        free(tmp->title);
        free(tmp->body);
        free(tmp);
    }
    if (discarded) {
        free(discarded->title);
        free(discarded->body);
        free(discarded);
    }
}

static Draft *find_draft(const char *title) {
    for (Draft *d = drafts; d; d = d->next)
        if (strcmp(d->title, title) == 0)
            return d;
    return NULL;
}

static void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *d = malloc(sizeof(Draft));
    if (!d) return;
    d->title = strdup(title);
    d->body = strdup(body);
    if (!d->title || !d->body) {
        free(d->title);
        free(d->body);
        free(d);
        return;
    }
    d->next = drafts;
    drafts = d;
}

static void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    char *newb = strdup(body);
    if (!newb) return;
    free(d->body);
    d->body = newb;
}

static void discard_draft(const char *title) {
    Draft *prev = NULL;
    for (Draft *d = drafts; d; d = d->next) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else drafts = d->next;
            // store as discarded
            if (discarded) {
                free(discarded->title);
                free(discarded->body);
                free(discarded);
            }
            discarded = d;
            d->next = NULL;
            return;
        }
        prev = d;
    }
}

static void restore_draft(void) {
    if (!discarded) return;
    if (find_draft(discarded->title)) {
        // title exists, cannot restore
        return;
    }
    discarded->next = drafts;
    drafts = discarded;
    discarded = NULL;
}

static void find_print(const char *title) {
    Draft *d = find_draft(title);
    if (d) printf("%s\n", d->body);
}

static void print_all(void) {
    for (Draft *d = drafts; d; d = d->next)
        printf("%s: %s\n", d->title, d->body);
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "CREATE") == 0) {
            if (*arg1) create_draft(arg1, arg2);
        } else if (strcmp(cmd, "REVISE") == 0) {
            if (*arg1) revise_draft(arg1, arg2);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            if (*arg1) discard_draft(arg1);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_print(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free(line);
    return 0;
}