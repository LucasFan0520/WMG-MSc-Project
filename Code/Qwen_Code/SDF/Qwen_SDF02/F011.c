// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *drafts = NULL;
Draft *discarded = NULL;

Draft* find_draft(const char *title) {
    for (Draft *d = drafts; d; d = d->next)
        if (strcmp(d->title, title) == 0) return d;
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "CREATE") == 0) {
            char title[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", title, &n) != 1) continue;
            char *body = rest + n;
            while (*body == ' ') body++;
            if (find_draft(title)) continue;
            Draft *d = malloc(sizeof(Draft));
            if (!d) continue;
            d->title = strdup(title);
            d->body = strdup(body);
            d->next = drafts;
            drafts = d;
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", title, &n) != 1) continue;
            char *body = rest + n;
            while (*body == ' ') body++;
            Draft *d = find_draft(title);
            if (d) {
                free(d->body);
                d->body = strdup(body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) != 1) continue;
            Draft *prev = NULL;
            for (Draft *d = drafts; d; prev = d, d = d->next) {
                if (strcmp(d->title, title) == 0) {
                    if (prev) prev->next = d->next;
                    else drafts = d->next;
                    if (discarded) {
                        free(discarded->title);
                        free(discarded->body);
                        free(discarded);
                    }
                    discarded = d;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (discarded) {
                discarded->next = drafts;
                drafts = discarded;
                discarded = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) != 1) continue;
            Draft *d = find_draft(title);
            if (d) print_text(d->body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Draft *d = drafts; d; d = d->next) {
                printf("%s ", d->title);
                print_text(d->body);
            }
        }
    }
    while (drafts) {
        Draft *d = drafts;
        drafts = drafts->next;
        free(d->title);
        free(d->body);
        free(d);
    }
    if (discarded) {
        free(discarded->title);
        free(discarded->body);
        free(discarded);
    }
    return 0;
}