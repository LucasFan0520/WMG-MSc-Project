// F026.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

int main(void) {
    Note *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *cat = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                text = p;
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Note *tmp = realloc(arr, capacity * sizeof(Note));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].category = strdup(cat);
            arr[count].text = strdup(text);
            if (!arr[count].category || !arr[count].text) exit(1);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *cat = line + 7;
            while (*cat == ' ') cat++;
            for (size_t i = 0; i < count; ) {
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
            while (*cat == ' ') cat++;
            for (size_t i = 0; i < count; ) {
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
            while (*word == ' ') word++;
            for (size_t i = 0; i < count; i++) {
                if (strstr(arr[i].text, word) != NULL) {
                    printf("[%s] %s\n", arr[i].category, arr[i].text);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("[%s] %s\n", arr[i].category, arr[i].text);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].category);
        free(arr[i].text);
    }
    free(arr);
    free(line);
    return 0;
}
