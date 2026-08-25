// F030.c
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
    char *old_str;
    char *new_str;
} Rule;

int main() {
    Rule *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *old_str = line + 4;
            char *space = strchr(old_str, ' ');
            if (space) {
                *space = '\0';
                char *new_str = space + 1;
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Rule));
                }
                arr[count].old_str = strdup(old_str);
                arr[count].new_str = strdup(new_str);
                count++;
            }
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            char *res = text;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(text, arr[i].old_str) == 0) {
                    res = arr[i].new_str;
                    break;
                }
            }
            printf("%s\n", res);
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old_str = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].old_str, old_str) == 0) {
                    free(arr[i].old_str);
                    free(arr[i].new_str);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                int i = atoi(p);
                int j = atoi(space + 1);
                if (i >= 0 && i < (int)count && j >= 0 && j < (int)count) {
                    Rule tmp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = tmp;
                }
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s -> %s\n", arr[i].old_str, arr[i].new_str);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].old_str);
        free(arr[i].new_str);
    }
    free(arr);
    return 0;
}
