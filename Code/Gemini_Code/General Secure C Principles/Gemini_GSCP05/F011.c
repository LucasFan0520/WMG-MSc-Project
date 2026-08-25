// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *drafts = NULL;
    int count = 0;
    int capacity = 0;
    Draft discarded = {NULL, NULL};
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *body_start = p + 1;
                while (*body_start == ' ') body_start++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *new_d = realloc(drafts, capacity * sizeof(Draft));
                    if (new_d) drafts = new_d;
                }
                if (count < capacity) {
                    drafts[count].title = strdup(title_start);
                    drafts[count].body = strdup(body_start);
                    count++;
                }
            }
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *body_start = p + 1;
                while (*body_start == ' ') body_start++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title_start) == 0) {
                        free(drafts[i].body);
                        drafts[i].body = strdup(body_start);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title_start = line + 8;
            while (*title_start == ' ') title_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title_start) == 0) {
                    free(discarded.title);
                    free(discarded.body);
                    discarded.title = drafts[i].title;
                    discarded.body = drafts[i].body;
                    for (int j = i; j < count - 1; j++) {
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
                    Draft *new_d = realloc(drafts, capacity * sizeof(Draft));
                    if (new_d) drafts = new_d;
                }
                if (count < capacity) {
                    drafts[count].title = discarded.title;
                    drafts[count].body = discarded.body;
                    count++;
                    discarded.title = NULL;
                    discarded.body = NULL;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title_start = line + 5;
            while (*title_start == ' ') title_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title_start) == 0) {
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
    free(discarded.title);
    free(discarded.body);
    return 0;
}
