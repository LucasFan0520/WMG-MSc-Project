// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Draft *arr = NULL;
    int count = 0;
    char *last_title = NULL;
    char *last_body = NULL;
    while (1) {
        char *line = read_line();
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
                arr = realloc(arr, (count + 1) * sizeof(Draft));
                arr[count].title = strdup(title);
                arr[count].body = strdup(body);
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
                    if (strcmp(arr[i].title, title) == 0) {
                        free(arr[i].body);
                        arr[i].body = strdup(body);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title = line + 8;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(last_title);
                    free(last_body);
                    last_title = strdup(arr[i].title);
                    last_body = strdup(arr[i].body);
                    free(arr[i].title);
                    free(arr[i].body);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (last_title) {
                arr = realloc(arr, (count + 1) * sizeof(Draft));
                arr[count].title = strdup(last_title);
                arr[count].body = strdup(last_body);
                count++;
                free(last_title);
                free(last_body);
                last_title = NULL;
                last_body = NULL;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    printf("%s %s\n", arr[i].title, arr[i].body);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].title, arr[i].body);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].body);
    }
    free(arr);
    free(last_title);
    free(last_body);
    return 0;
}
