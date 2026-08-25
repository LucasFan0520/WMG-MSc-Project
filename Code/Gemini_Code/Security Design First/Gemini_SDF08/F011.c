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
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    Draft discarded = {NULL, NULL};

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *p = line + 7;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *title = p;
                char *body = space + 1;
                Draft *tmp = realloc(drafts, sizeof(Draft) * (count + 1));
                if (tmp) {
                    drafts = tmp;
                    drafts[count].title = strdup(title);
                    drafts[count].body = strdup(body);
                    if (drafts[count].title && drafts[count].body) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *p = line + 7;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *title = p;
                char *body = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        char *nb = strdup(body);
                        if (nb) {
                            free(drafts[i].body);
                            drafts[i].body = nb;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title = line + 8;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(discarded.title);
                free(discarded.body);
                discarded.title = drafts[found].title;
                discarded.body = drafts[found].body;
                for (int i = found; i < count - 1; i++) {
                    drafts[i] = drafts[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (discarded.title && discarded.body) {
                Draft *tmp = realloc(drafts, sizeof(Draft) * (count + 1));
                if (tmp) {
                    drafts = tmp;
                    drafts[count].title = discarded.title;
                    drafts[count].body = discarded.body;
                    count++;
                    discarded.title = NULL;
                    discarded.body = NULL;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    printf("%s\n", drafts[i].body);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    free(discarded.title);
    free(discarded.body);
    free(line);
    return 0;
}
