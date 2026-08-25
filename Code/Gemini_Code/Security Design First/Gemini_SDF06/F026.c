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
    Note *notes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *cat = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;

            if (*cat == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Note *new_notes = realloc(notes, new_cap * sizeof(Note));
                if (!new_notes) continue;
                notes = new_notes;
                capacity = new_cap;
            }
            char *c = strdup(cat);
            char *t = strdup(text);
            if (c && t) {
                notes[count].category = c;
                notes[count].text = t;
                count++;
            } else {
                free(c);
                free(t);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *cat = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*cat == '\0') continue;

            size_t i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, cat) == 0) {
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
            while (*p == ' ' || *p == '\t') p++;
            char *cat = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*cat == '\0') continue;

            size_t i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, cat) != 0) {
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
            while (*p == ' ' || *p == '\t') p++;
            char *word = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*word == '\0') continue;

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
