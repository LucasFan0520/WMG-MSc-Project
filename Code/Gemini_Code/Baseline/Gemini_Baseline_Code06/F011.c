// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main() {
    Draft *drafts = NULL;
    int count = 0;
    Draft last_discarded = {NULL, NULL};
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *p = line + 7;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *body = p + 1;
                drafts = realloc(drafts, (count + 1) * sizeof(Draft));
                drafts[count].title = strdup(title);
                drafts[count].body = strdup(body);
                count++;
            }
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *p = line + 7;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *body = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        free(drafts[i].body);
                        drafts[i].body = strdup(body);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title = line + 8;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    if (last_discarded.title) {
                        free(last_discarded.title);
                        free(last_discarded.body);
                    }
                    last_discarded.title = strdup(drafts[i].title);
                    last_discarded.body = strdup(drafts[i].body);
                    free(drafts[i].title);
                    free(drafts[i].body);
                    for (int j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    drafts = realloc(drafts, count * sizeof(Draft));
                    break;
                }
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (last_discarded.title) {
                drafts = realloc(drafts, (count + 1) * sizeof(Draft));
                drafts[count].title = strdup(last_discarded.title);
                drafts[count].body = strdup(last_discarded.body);
                count++;
                free(last_discarded.title);
                free(last_discarded.body);
                last_discarded.title = NULL;
                last_discarded.body = NULL;
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
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    if (last_discarded.title) {
        free(last_discarded.title);
        free(last_discarded.body);
    }
    return 0;
}
