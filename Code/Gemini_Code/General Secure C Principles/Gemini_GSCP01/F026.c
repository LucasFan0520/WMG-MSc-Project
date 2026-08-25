// F026.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} FilteredNote;

int main(void) {
    FilteredNote *notes = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *category = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *text = p;
            FilteredNote *temp = realloc(notes, (count + 1) * sizeof(FilteredNote));
            if (temp) {
                notes = temp;
                notes[count].category = strdup(category);
                notes[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *category = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, category) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *category = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, category) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *word = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strstr(notes[i].text, word) != NULL) {
                    printf("[%s] %s\n", notes[i].category, notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("[%s] %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    free(line);
    return 0;
}
