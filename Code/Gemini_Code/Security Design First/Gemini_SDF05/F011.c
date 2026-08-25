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
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    Draft last_discarded = {NULL, NULL};

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            char *body = "";
            if (space) {
                *space = '\0';
                body = space + 1;
                while (*body == ' ') body++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Draft *tmp = realloc(drafts, new_cap * sizeof(Draft));
                if (!tmp) return 1;
                drafts = tmp;
                capacity = new_cap;
            }
            char *nt = strdup(title);
            char *nb = strdup(body);
            if (!nt || !nb) return 1;
            drafts[count].title = nt;
            drafts[count].body = nb;
            count++;
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            char *body = "";
            if (space) {
                *space = '\0';
                body = space + 1;
                while (*body == ' ') body++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *nb = strdup(body);
                if (!nb) return 1;
                free(drafts[found].body);
                drafts[found].body = nb;
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title = line + 8;
            while (*title == ' ') title++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(last_discarded.title);
                free(last_discarded.body);
                last_discarded.title = drafts[found].title;
                last_discarded.body = drafts[found].body;
                for (int i = found; i < count - 1; i++) {
                    drafts[i] = drafts[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (last_discarded.title) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Draft *tmp = realloc(drafts, new_cap * sizeof(Draft));
                    if (!tmp) return 1;
                    drafts = tmp;
                    capacity = new_cap;
                }
                drafts[count].title = last_discarded.title;
                drafts[count].body = last_discarded.body;
                count++;
                last_discarded.title = NULL;
                last_discarded.body = NULL;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%s\n", drafts[found].body);
            else printf("NOT_FOUND\n");
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
    free(last_discarded.title);
    free(last_discarded.body);
    return 0;
}
