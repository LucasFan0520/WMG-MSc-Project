/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *head = NULL;
Draft *tail = NULL;

Draft *discarded = NULL; // most recently discarded

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
    if (!d) return;
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = NULL;
    if (tail) tail->next = d;
    else head = d;
    tail = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = strdup(body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            // store in discarded
            if (discarded) {
                free(discarded->title);
                free(discarded->body);
                free(discarded);
            }
            discarded = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void restore_draft(void) {
    if (!discarded) return;
    if (find_draft(discarded->title)) {
        // title conflict, ignore
        return;
    }
    // re-add to end
    discarded->next = NULL;
    if (tail) tail->next = discarded;
    else head = discarded;
    tail = discarded;
    discarded = NULL;
}

void find_draft_cmd(const char *title) {
    Draft *d = find_draft(title);
    if (!d) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s\n", d->title, d->body);
}

void print_drafts(void) {
    Draft *d = head;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

void free_all(void) {
    Draft *d = head;
    while (d) {
        Draft *tmp = d;
        d = d->next;
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

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char title[256];
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) {
                char *body = p;
                while (*body && *body != ' ') body++;
                while (*body && *body == ' ') body++;
                if (*body) {
                    create_draft(title, body);
                }
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[256];
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) {
                char *body = p;
                while (*body && *body != ' ') body++;
                while (*body && *body == ' ') body++;
                if (*body) {
                    revise_draft(title, body);
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            discard_draft(p);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_draft_cmd(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    free_all();
    return 0;
}