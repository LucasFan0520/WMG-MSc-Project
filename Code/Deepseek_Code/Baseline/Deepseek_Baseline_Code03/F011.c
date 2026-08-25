// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct draft {
    char *title;
    char *body;
    struct draft *next;
};

struct draft *drafts = NULL;
struct draft *last_discarded = NULL;

struct draft *find_draft(char *title) {
    struct draft *d = drafts;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void create_draft(char *title, char *body) {
    struct draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = malloc(strlen(body) + 1);
        strcpy(d->body, body);
    } else {
        d = malloc(sizeof(struct draft));
        d->title = malloc(strlen(title) + 1);
        strcpy(d->title, title);
        d->body = malloc(strlen(body) + 1);
        strcpy(d->body, body);
        d->next = drafts;
        drafts = d;
    }
}

void revise_draft(char *title, char *body) {
    struct draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = malloc(strlen(body) + 1);
        strcpy(d->body, body);
    }
}

void discard_draft(char *title) {
    struct draft *d = drafts, *prev = NULL;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else drafts = d->next;
            // store for restore
            if (last_discarded) {
                free(last_discarded->title);
                free(last_discarded->body);
                free(last_discarded);
            }
            last_discarded = d;
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft() {
    if (last_discarded) {
        // add back to drafts
        last_discarded->next = drafts;
        drafts = last_discarded;
        last_discarded = NULL;
    }
}

void find_draft_print(char *title) {
    struct draft *d = find_draft(title);
    if (d) printf("%s\n", d->body);
}

void print_drafts() {
    struct draft *d = drafts;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], body[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, title, body);
        if (strcmp(cmd, "CREATE") == 0 && n >= 3) {
            create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0 && n >= 3) {
            revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0 && n >= 2) {
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    return 0;
}