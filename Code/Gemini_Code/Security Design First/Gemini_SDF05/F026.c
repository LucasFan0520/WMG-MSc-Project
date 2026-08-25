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
    int count = 0;
    int capacity = 0;
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
            char *category = p;
            char *space = strchr(category, ' ');
            char *text = "";
            if (space) {
                *space = '\0';
                text = space + 1;
                while (*text == ' ') text++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Note *tmp = realloc(notes, new_cap * sizeof(Note));
                if (!tmp) return 1;
                notes = tmp;
                capacity = new_cap;
            }
            notes[count].category = strdup(category);
            notes[count].text = strdup(text);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *category = line + 7;
            while (*category == ' ') category++;
            int w = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(notes[i].category, category) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                } else {
                    notes[w++] = notes[i];
                }
            }
            count = w;
        } else if (strncmp(line, "KEEP ", 5) == 0) {
            char *category = line + 5;
            while (*category == ' ') category++;
            int w = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(notes[i].category, category) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                } else {
                    notes[w++] = notes[i];
                }
            }
            count = w;
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *word = line + 5;
            while (*word == ' ') word++;
            for (int i = 0; i < count; i++) {
                if (strstr(notes[i].text, word) != NULL) {
                    printf("%s: %s\n", notes[i].category, notes[i].text);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}
