// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

int main() {
    Note *notes = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *cat = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (cat && text) {
                notes = realloc(notes, (count + 1) * sizeof(Note));
                notes[count].category = strdup(cat);
                notes[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = strtok(NULL, " ");
            if (cat) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(notes[i].category, cat) == 0) {
                        free(notes[i].category);
                        free(notes[i].text);
                        for (int j = i; j < count - 1; j++) {
                            notes[j] = notes[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(notes);
                    notes = NULL;
                } else {
                    notes = realloc(notes, count * sizeof(Note));
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *cat = strtok(NULL, " ");
            if (cat) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(notes[i].category, cat) != 0) {
                        free(notes[i].category);
                        free(notes[i].text);
                        for (int j = i; j < count - 1; j++) {
                            notes[j] = notes[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(notes);
                    notes = NULL;
                } else {
                    notes = realloc(notes, count * sizeof(Note));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = strtok(NULL, " ");
            if (word) {
                for (int i = 0; i < count; i++) {
                    if (strstr(notes[i].text, word) != NULL) {
                        printf("[%s] %s\n", notes[i].category, notes[i].text);
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("[%s] %s\n", notes[i].category, notes[i].text);
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
