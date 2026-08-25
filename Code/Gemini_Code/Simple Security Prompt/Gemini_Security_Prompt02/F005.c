// F005.c
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

int main() {
    char **arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            int idx = atoi(p);
            char *space = strchr(p, ' ');
            if (space && idx >= 0 && idx <= (int)count) {
                char *text = space + 1;
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(char *));
                }
                for (int i = (int)count; i > idx; i--) {
                    arr[i] = arr[i - 1];
                }
                arr[idx] = strdup(text);
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                arr = realloc(arr, cap * sizeof(char *));
            }
            arr[count] = strdup(text);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            int idx = atoi(p);
            if (idx >= 0 && idx < (int)count) {
                free(arr[idx]);
                for (size_t i = idx; i < count - 1; i++) {
                    arr[i] = arr[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *p = line + 6;
            int idx = atoi(p);
            char *space = strchr(p, ' ');
            if (space && idx >= 0 && idx < (int)count) {
                char *text = space + 1;
                free(arr[idx]);
                arr[idx] = strdup(text);
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                char *t = arr[i];
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i]);
    }
    free(arr);
    return 0;
}
