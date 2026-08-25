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

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *title = args;
                char *body = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *next = realloc(drafts, capacity * sizeof(Draft));
                    if (!next) break;
                    drafts = next;
                }
                drafts[count].title = strdup(title);
                drafts[count].body = strdup(body);
                count++;
            }
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *title = args;
                char *body = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        free(drafts[i].body);
                        drafts[i].body = strdup(body);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title = line + 8;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    free(discarded.title);
                    free(discarded.body);
                    discarded.title = drafts[i].title;
                    discarded.body = drafts[i].body;
                    for (size_t j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (discarded.title) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *next = realloc(drafts, capacity * sizeof(Draft));
                    if (next) drafts = next;
                }
                drafts[count].title = discarded.title;
                drafts[count].body = discarded.body;
                count++;
                discarded.title = NULL;
                discarded.body = NULL;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    printf("%s %s\n", drafts[i].title, drafts[i].body);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    free(discarded.title);
    free(discarded.body);
    return 0;
}
