// F026.c
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
    char *category;
    char *text;
} Note;

int main() {
    Note *arr = NULL;
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
            char *cat = line + 4;
            char *space = strchr(cat, ' ');
            if (space) {
                *space = '\0';
                char *text = space + 1;
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Note));
                }
                arr[count].category = strdup(cat);
                arr[count].text = strdup(text);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *cat = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].category, cat) == 0) {
                    free(arr[i].category);
                    free(arr[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "KEEP ", 5) == 0) {
            char *cat = line + 5;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].category, cat) != 0) {
                    free(arr[i].category);
                    free(arr[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *word = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strstr(arr[i].text, word) != NULL) {
                    printf("%s: %s\n", arr[i].category, arr[i].text);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", arr[i].category, arr[i].text);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].category);
        free(arr[i].text);
    }
    free(arr);
    return 0;
}
