// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

int main() {
    Draft *drafts = NULL;
    int count = 0;
    Draft discarded = {NULL, NULL};
    int has_discarded = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (title && body) {
                drafts = realloc(drafts, (count + 1) * sizeof(Draft));
                drafts[count].title = strdup(title);
                drafts[count].body = strdup(body);
                count++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (title && body) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        free(drafts[i].body);
                        drafts[i].body = strdup(body);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = strtok(NULL, " ");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        if (has_discarded) {
                            free(discarded.title);
                            free(discarded.body);
                        }
                        discarded.title = strdup(drafts[i].title);
                        discarded.body = strdup(drafts[i].body);
                        has_discarded = 1;
                        free(drafts[i].title);
                        free(drafts[i].body);
                        for (int j = i; j < count - 1; j++) {
                            drafts[j] = drafts[j + 1];
                        }
                        count--;
                        if (count == 0) {
                            free(drafts);
                            drafts = NULL;
                        } else {
                            drafts = realloc(drafts, count * sizeof(Draft));
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (has_discarded) {
                drafts = realloc(drafts, (count + 1) * sizeof(Draft));
                drafts[count].title = strdup(discarded.title);
                drafts[count].body = strdup(discarded.body);
                count++;
                free(discarded.title);
                free(discarded.body);
                discarded.title = NULL;
                discarded.body = NULL;
                has_discarded = 0;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        printf("%s\n", drafts[i].body);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
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
    if (has_discarded) {
        free(discarded.title);
        free(discarded.body);
    }
    return 0;
}
