/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

typedef struct Trash {
    char *title;
    char *body;
    int has_trash;
} Trash;

Draft *drafts = NULL;
Trash trash = {NULL, NULL, 0};

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
    if (!d) return;
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
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else drafts = d->next;
            if (trash.has_trash) {
                free(trash.title);
                free(trash.body);
            }
            trash.title = strdup(d->title);
            trash.body = strdup(d->body);
            trash.has_trash = 1;
            free(d->title);
            free(d->body);
            free(d);
            return;
        }
        prev = d;
        d = d->next;
    }
}

void restore_draft(void) {
    if (!trash.has_trash) return;
    if (find_draft(trash.title)) {
        free(trash.title);
        free(trash.body);
        trash.has_trash = 0;
        return;
    }
    Draft *d = malloc(sizeof(Draft));
    if (!d) return;
    d->title = strdup(trash.title);
    d->body = strdup(trash.body);
    d->next = drafts;
    drafts = d;
    free(trash.title);
    free(trash.body);
    trash.has_trash = 0;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s\n", d->title, d->body);
}

void print_all(void) {
    Draft *d = drafts;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char *body = line + strlen(cmd) + strlen(arg1) + 2;
            while (*body == ' ') body++;
            create_draft(arg1, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *body = line + strlen(cmd) + strlen(arg1) + 2;
            while (*body == ' ') body++;
            revise_draft(arg1, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            discard_draft(arg1);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            find_draft_print(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    while (drafts) {
        Draft *tmp = drafts;
        drafts = drafts->next;
        free(tmp->title);
        free(tmp->body);
        free(tmp);
    }
    if (trash.has_trash) {
        free(trash.title);
        free(trash.body);
    }
    return 0;
}