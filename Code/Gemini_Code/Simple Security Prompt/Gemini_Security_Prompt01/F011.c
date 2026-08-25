// F011.c
#define _GNU_SOURCE
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    char *discarded_title = NULL;
    char *discarded_body = NULL;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "CREATE") == 0 && args) {
            char *title = args;
            char *space2 = strchr(args, ' ');
            char *body = "";
            if (space2) {
                *space2 = '\0';
                body = space2 + 1;
                while (*body == ' ') body++;
            }
            drafts = realloc(drafts, (count + 1) * sizeof(Draft));
            if (!drafts) return 1;
            drafts[count].title = strdup(title);
            drafts[count].body = strdup(body);
            count++;
        } else if (strcmp(cmd, "REVISE") == 0 && args) {
            char *title = args;
            char *space2 = strchr(args, ' ');
            char *body = "";
            if (space2) {
                *space2 = '\0';
                body = space2 + 1;
                while (*body == ' ') body++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    free(drafts[i].body);
                    drafts[i].body = strdup(body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, args) == 0) {
                    free(discarded_title);
                    free(discarded_body);
                    discarded_title = strdup(drafts[i].title);
                    discarded_body = strdup(drafts[i].body);

                    free(drafts[i].title);
                    free(drafts[i].body);
                    for (int j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    drafts = realloc(drafts, count * sizeof(Draft));
                    if (count > 0 && !drafts) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (discarded_title) {
                drafts = realloc(drafts, (count + 1) * sizeof(Draft));
                if (!drafts) return 1;
                drafts[count].title = strdup(discarded_title);
                drafts[count].body = strdup(discarded_body);
                count++;

                free(discarded_title);
                free(discarded_body);
                discarded_title = NULL;
                discarded_body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, args) == 0) {
                    printf("%s\n", drafts[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }

    free(line);
    free(discarded_title);
    free(discarded_body);
    for (int i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    return 0;
}
