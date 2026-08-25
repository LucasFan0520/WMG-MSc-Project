// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

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
    Note *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                arr = realloc(arr, (count + 1) * sizeof(Note));
                arr[count].category = strdup(cat);
                arr[count].text = strdup(text);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *cat = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(arr[i].category, cat) == 0) {
                    free(arr[i].category);
                    free(arr[i].text);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "KEEP ", 5) == 0) {
            char *cat = line + 5;
            int i = 0;
            while (i < count) {
                if (strcmp(arr[i].category, cat) != 0) {
                    free(arr[i].category);
                    free(arr[i].text);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *word = line + 5;
            for (int i = 0; i < count; i++) {
                if (strstr(arr[i].text, word) != NULL) {
                    printf("%s %s\n", arr[i].category, arr[i].text);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].category, arr[i].text);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].category);
        free(arr[i].text);
    }
    free(arr);
    return 0;
}
