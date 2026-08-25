/* F011.c */
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
    int capacity = 0;
    char *disc_title = NULL;
    char *disc_body = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            char *t = NULL;
            char *b = NULL;
            if (space) {
                *space = '\0';
                t = strdup(args);
                b = strdup(space + 1);
                *space = ' ';
            } else {
                t = strdup(args);
                b = strdup("");
            }
            if (t && b) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Draft *tmp = realloc(drafts, new_cap * sizeof(Draft));
                    if (tmp) {
                        drafts = tmp;
                        capacity = new_cap;
                    } else {
                        free(t);
                        free(b);
                        continue;
                    }
                }
                drafts[count].title = t;
                drafts[count].body = b;
                count++;
            } else {
                free(t);
                free(b);
            }
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *t = args;
                char *b = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, t) == 0) {
                        char *n_body = strdup(b);
                        if (n_body) {
                            free(drafts[i].body);
                            drafts[i].body = n_body;
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *t = line + 8;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, t) == 0) {
                    free(disc_title);
                    free(disc_body);
                    disc_title = drafts[i].title;
                    disc_body = drafts[i].body;
                    for (int j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (disc_title && disc_body) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Draft *tmp = realloc(drafts, new_cap * sizeof(Draft));
                    if (tmp) {
                        drafts = tmp;
                        capacity = new_cap;
                    } else {
                        continue;
                    }
                }
                drafts[count].title = disc_title;
                drafts[count].body = disc_body;
                count++;
                disc_title = NULL;
                disc_body = NULL;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *t = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, t) == 0) {
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
    free(line);
    for (int i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    free(disc_title);
    free(disc_body);
    return 0;
}
