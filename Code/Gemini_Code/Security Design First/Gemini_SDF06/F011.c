// F011.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *drafts = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Draft discarded = {NULL, NULL};
    int has_discarded = 0;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "CREATE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *body = p;

            if (*title == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Draft *new_drafts = realloc(drafts, new_cap * sizeof(Draft));
                if (!new_drafts) continue;
                drafts = new_drafts;
                capacity = new_cap;
            }
            char *t = strdup(title);
            char *b = strdup(body);
            if (t && b) {
                drafts[count].title = t;
                drafts[count].body = b;
                count++;
            } else {
                free(t);
                free(b);
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *body = p;

            if (*title == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    char *b = strdup(body);
                    if (b) {
                        free(drafts[i].body);
                        drafts[i].body = b;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*title == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    if (has_discarded) {
                        free(discarded.title);
                        free(discarded.body);
                    }
                    discarded.title = drafts[i].title;
                    discarded.body = drafts[i].body;
                    has_discarded = 1;

                    for (size_t j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (has_discarded) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Draft *new_drafts = realloc(drafts, new_cap * sizeof(Draft));
                    if (!new_drafts) continue;
                    drafts = new_drafts;
                    capacity = new_cap;
                }
                drafts[count].title = discarded.title;
                drafts[count].body = discarded.body;
                count++;
                has_discarded = 0;
                discarded.title = NULL;
                discarded.body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*title == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    printf("%s\n", drafts[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    if (has_discarded) {
        free(discarded.title);
        free(discarded.body);
    }
    free(line);
    return 0;
}
