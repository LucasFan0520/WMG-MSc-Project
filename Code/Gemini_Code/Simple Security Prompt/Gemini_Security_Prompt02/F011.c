// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    Draft *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    char *hist_title = NULL;
    char *hist_body = NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *t_start = line + 7;
            char *space = strchr(t_start, ' ');
            char *b_start = "";
            if (space) {
                *space = '\0';
                b_start = space + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                arr = realloc(arr, cap * sizeof(Draft));
            }
            arr[count].title = strdup(t_start);
            arr[count].body = strdup(b_start);
            count++;
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *t_start = line + 7;
            char *space = strchr(t_start, ' ');
            char *b_start = "";
            if (space) {
                *space = '\0';
                b_start = space + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, t_start) == 0) {
                    free(arr[i].body);
                    arr[i].body = strdup(b_start);
                    break;
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *t_start = line + 8;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, t_start) == 0) {
                    free(hist_title);
                    free(hist_body);
                    hist_title = strdup(arr[i].title);
                    hist_body = strdup(arr[i].body);
                    free(arr[i].title);
                    free(arr[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (hist_title) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Draft));
                }
                arr[count].title = strdup(hist_title);
                arr[count].body = strdup(hist_body);
                count++;
                free(hist_title);
                free(hist_body);
                hist_title = NULL;
                hist_body = NULL;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *t_start = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, t_start) == 0) {
                    printf("%s %s\n", arr[i].title, arr[i].body);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].title, arr[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].body);
    }
    free(arr);
    free(hist_title);
    free(hist_body);
    return 0;
}
