/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
} Draft;

static Draft *drafts = NULL;
static size_t dcount = 0, dcap = 0;

static Draft *find_draft(const char *title) {
    for (size_t i = 0; i < dcount; i++) {
        if (strcmp(drafts[i].title, title) == 0)
            return &drafts[i];
    }
    return NULL;
}

static void add_draft(const char *title, const char *body) {
    if (dcount == dcap) {
        dcap = dcap ? dcap * 2 : 4;
        drafts = realloc(drafts, dcap * sizeof(Draft));
        if (!drafts) { perror("realloc"); exit(1); }
    }
    drafts[dcount].title = strdup(title);
    drafts[dcount].body = strdup(body);
    dcount++;
}

static void remove_draft(const char *title) {
    for (size_t i = 0; i < dcount; i++) {
        if (strcmp(drafts[i].title, title) == 0) {
            free(drafts[i].title);
            free(drafts[i].body);
            for (size_t j = i; j < dcount - 1; j++)
                drafts[j] = drafts[j + 1];
            dcount--;
            return;
        }
    }
}

static Draft *last_discarded = NULL;

static void discard_draft(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    // free previous last discarded
    if (last_discarded) {
        free(last_discarded->title);
        free(last_discarded->body);
        free(last_discarded);
    }
    last_discarded = malloc(sizeof(Draft));
    if (!last_discarded) { perror("malloc"); exit(1); }
    last_discarded->title = strdup(d->title);
    last_discarded->body = strdup(d->body);
    remove_draft(title);
}

static void restore_draft(void) {
    if (!last_discarded) return;
    add_draft(last_discarded->title, last_discarded->body);
    free(last_discarded->title);
    free(last_discarded->body);
    free(last_discarded);
    last_discarded = NULL;
}

static void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (d) {
        printf("%s\n%s\n", d->title, d->body);
    }
}

static void print_all(void) {
    for (size_t i = 0; i < dcount; i++) {
        printf("%s\n%s\n", drafts[i].title, drafts[i].body);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *body = strtok(NULL, "");
            if (!body) body = "";
            // if exists, replace
            Draft *d = find_draft(title);
            if (d) {
                free(d->body);
                d->body = strdup(body);
            } else {
                add_draft(title, body);
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *body = strtok(NULL, "");
            if (!body) body = "";
            Draft *d = find_draft(title);
            if (d) {
                free(d->body);
                d->body = strdup(body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = strtok(NULL, " ");
            if (title) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    for (size_t i = 0; i < dcount; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    if (last_discarded) {
        free(last_discarded->title);
        free(last_discarded->body);
        free(last_discarded);
    }
    return 0;
}