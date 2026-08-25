// F011.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

Draft *drafts = NULL;
int count = 0;
int capacity = 0;

char *discarded_title = NULL;
char *discarded_body = NULL;

int find_draft(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(drafts[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "CREATE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *body = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Draft *tmp = realloc(drafts, capacity * sizeof(Draft));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        drafts = tmp;
                    }
                    drafts[count].title = strdup(arg1);
                    drafts[count].body = strdup(body);
                    if (drafts[count].title && drafts[count].body) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "REVISE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *body = space2 + 1;
                    int idx = find_draft(arg1);
                    if (idx != -1) {
                        char *nb = strdup(body);
                        if (nb) {
                            free(drafts[idx].body);
                            drafts[idx].body = nb;
                        }
                    }
                }
            } else if (strcmp(cmd, "DISCARD") == 0) {
                int idx = find_draft(arg1);
                if (idx != -1) {
                    free(discarded_title);
                    free(discarded_body);
                    discarded_title = strdup(drafts[idx].title);
                    discarded_body = strdup(drafts[idx].body);
                    free(drafts[idx].title);
                    free(drafts[idx].body);
                    for (int i = idx; i < count - 1; i++) {
                        drafts[i] = drafts[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                int idx = find_draft(arg1);
                if (idx != -1) {
                    printf("%s\n", drafts[idx].body);
                }
            }
        } else {
            if (strcmp(cmd, "RESTORE") == 0) {
                if (discarded_title && discarded_body) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Draft *tmp = realloc(drafts, capacity * sizeof(Draft));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        drafts = tmp;
                    }
                    drafts[count].title = strdup(discarded_title);
                    drafts[count].body = strdup(discarded_body);
                    if (drafts[count].title && drafts[count].body) {
                        count++;
                    }
                    free(discarded_title);
                    free(discarded_body);
                    discarded_title = NULL;
                    discarded_body = NULL;
                }
            } else if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", drafts[i].title, drafts[i].body);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    free(discarded_title);
    free(discarded_body);
    return 0;
}