// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main() {
    Draft *drafts = NULL;
    int count = 0;
    char *last_discard_title = NULL;
    char *last_discard_body = NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "RESTORE") == 0) {
                if (last_discard_title && last_discard_body) {
                    drafts = realloc(drafts, sizeof(Draft) * (count + 1));
                    drafts[count].title = strdup(last_discard_title);
                    drafts[count].body = strdup(last_discard_body);
                    count++;
                    free(last_discard_title);
                    free(last_discard_body);
                    last_discard_title = NULL;
                    last_discard_body = NULL;
                }
            } else if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", drafts[i].title, drafts[i].body);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "CREATE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = args;
                char *body = p2 + 1;
                drafts = realloc(drafts, sizeof(Draft) * (count + 1));
                drafts[count].title = strdup(title);
                drafts[count].body = strdup(body);
                count++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = args;
                char *body = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        free(drafts[i].body);
                        drafts[i].body = strdup(body);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    free(last_discard_title);
                    free(last_discard_body);
                    last_discard_title = strdup(drafts[i].title);
                    last_discard_body = strdup(drafts[i].body);
                    free(drafts[i].title);
                    free(drafts[i].body);
                    for (int j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    if (count == 0) { free(drafts); drafts = NULL; }
                    else { drafts = realloc(drafts, sizeof(Draft) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    printf("%s\n", drafts[i].body);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    free(last_discard_title);
    free(last_discard_body);
    return 0;
}
