/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *drafts = NULL;
Draft *trash = NULL;  

Draft *find_draft(const char *title) {
    Draft *d = drafts;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void free_draft(Draft *d) {
    free(d->title);
    free(d->body);
    free(d);
}

void create_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = strdup(body);
        return;
    }
    d = malloc(sizeof(Draft));
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
    Draft *prev = NULL, *curr = drafts;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else drafts = curr->next;
            if (trash) free_draft(trash);
            trash = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void restore_draft(void) {
    if (!trash) return;
    trash->next = drafts;
    drafts = trash;
    trash = NULL;
}

void find_draft_print(const char *title) {
    Draft *d = find_draft(title);
    if (!d) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", d->body);
}

void print_drafts(void) {
    Draft *d = drafts;
    while (d) {
        printf("%s\n", d->title);
        d = d->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[8192];
    char cmd[32];
    char title[256];
    char body[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "CREATE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) != 1) continue;
            p += strlen(title);
            while (*p == ' ') p++;
            create_draft(title, p);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) != 1) continue;
            p += strlen(title);
            while (*p == ' ') p++;
            revise_draft(title, p);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) find_draft_print(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }

    while (drafts) {
        Draft *tmp = drafts;
        drafts = drafts->next;
        free_draft(tmp);
    }
    if (trash) free_draft(trash);
    return 0;
}