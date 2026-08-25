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
Draft *tail = NULL;
Draft *discarded = NULL; // only one

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
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = NULL;
    if (tail) {
        tail->next = d;
        tail = d;
    } else {
        head = tail = d;
    }
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = strdup(body);
}

void discard_draft(const char *title) {
    Draft *prev = NULL;
    Draft *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else head = d->next;
            if (!d->next) tail = prev;
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

void restore_draft() {
    if (!discarded) return;
    discarded->next = NULL;
    if (tail) {
        tail->next = discarded;
        tail = discarded;
    } else {
        head = tail = discarded;
    }
    discarded = NULL;
}

void find_draft_and_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s:", d->title);
    for (char *s = d->body; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void print_all_drafts() {
    Draft *d = head;
    while (d) {
        printf("%s:", d->title);
        for (char *s = d->body; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        d = d->next;
    }
}

void free_all() {
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "CREATE") == 0) {
            char title[256], body[4096];
            if (sscanf(rest, "%255s %[^\n]", title, body) == 2) {
                create_draft(title, body);
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[256], body[4096];
            if (sscanf(rest, "%255s %[^\n]", title, body) == 2) {
                revise_draft(title, body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) {
                discard_draft(title);
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) {
                find_draft_and_print(title);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_drafts();
        }
    }
    free_all();
    return 0;
}