// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

Note *notes = NULL;
int count = 0;
int capacity = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *text = sp + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    notes = realloc(notes, capacity * sizeof(Note));
                }
                notes[count].category = strdup(p1);
                notes[count].text = strdup(text);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *category = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, category) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "KEEP ", 5) == 0) {
            char *category = line + 5;
            int i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, category) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *word = line + 5;
            for (int i = 0; i < count; i++) {
                if (strstr(notes[i].text, word) != NULL) {
                    printf("%s %s\n", notes[i].category, notes[i].text);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}
