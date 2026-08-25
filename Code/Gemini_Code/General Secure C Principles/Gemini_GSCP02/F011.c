// F011.c
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
    char *disc_title = NULL;
    char *disc_body = NULL;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char title[256];
            char body[1024];
            if (sscanf(line + 7, "%255s %[^\n]", title, body) == 2) {
                Draft *tmp = realloc(drafts, (count + 1) * sizeof(Draft));
                if (tmp) {
                    drafts = tmp;
                    drafts[count].title = strdup(title);
                    drafts[count].body = strdup(body);
                    count++;
                }
            }
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char title[256];
            char body[1024];
            if (sscanf(line + 7, "%255s %[^\n]", title, body) == 2) {
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
            while (*title == ' ') {
                title++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    free(disc_title);
                    free(disc_body);
                    disc_title = strdup(drafts[i].title);
                    disc_body = strdup(drafts[i].body);
                    free(drafts[i].title);
                    free(drafts[i].body);
                    for (int j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (disc_title && disc_body) {
                Draft *tmp = realloc(drafts, (count + 1) * sizeof(Draft));
                if (tmp) {
                    drafts = tmp;
                    drafts[count].title = strdup(disc_title);
                    drafts[count].body = strdup(disc_body);
                    count++;
                    free(disc_title);
                    free(disc_body);
                    disc_title = NULL;
                    disc_body = NULL;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') {
                title++;
            }
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
