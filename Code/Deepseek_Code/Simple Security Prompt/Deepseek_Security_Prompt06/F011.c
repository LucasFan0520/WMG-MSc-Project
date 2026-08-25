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
Draft *discarded = NULL; // only one most recently discarded

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
    Draft *d = head, *prev = NULL;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else head = d->next;
            // store as discarded, free old discarded if any
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
        d = d->next;
    }
}

void restore_draft(void) {
    if (!discarded) return;
    // insert at head
    discarded->next = head;
    head = discarded;
    discarded = NULL;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    printf("%s %s\n", d->title, d->body);
}

void print_all(void) {
    Draft *d = head;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

void free_all(void) {
    Draft *d = head;
    while (d) {
        Draft *tmp = d->next;
        free(d->title);
        free(d->body);
        free(d);
        d = tmp;
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], body[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "CREATE") == 0) {
            if (sscanf(rest, "%255s", title) == 1) {
                const char *b = rest + strlen(title) + 1;
                while (*b == ' ') b++;
                if (*b) create_draft(title, b);
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            if (sscanf(rest, "%255s", title) == 1) {
                const char *b = rest + strlen(title) + 1;
                while (*b == ' ') b++;
                if (*b) revise_draft(title, b);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            if (sscanf(rest, "%255s", title) == 1) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", title) == 1) find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}