// F026.c
#define _GNU_SOURCE
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *cat = args;
            char *space2 = strchr(args, ' ');
            char *text = "";
            if (space2) {
                *space2 = '\0';
                text = space2 + 1;
                while (*text == ' ') text++;
            }
            notes = realloc(notes, (count + 1) * sizeof(Note));
            if (!notes) return 1;
            notes[count].category = strdup(cat);
            notes[count].text = strdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            int idx = 0;
            while (idx < count) {
                if (strcmp(notes[idx].category, args) == 0) {
                    free(notes[idx].category);
                    free(notes[idx].text);
                    for (int j = idx; j < count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
            notes = realloc(notes, count * sizeof(Note));
            if (count > 0 && !notes) return 1;
        } else if (strcmp(cmd, "KEEP") == 0 && args) {
            int idx = 0;
            while (idx < count) {
                if (strcmp(notes[idx].category, args) != 0) {
                    free(notes[idx].category);
                    free(notes[idx].text);
                    for (int j = idx; j < count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
            notes = realloc(notes, count * sizeof(Note));
            if (count > 0 && !notes) return 1;
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strstr(notes[i].text, args) != NULL) {
                    printf("%s: %s\n", notes[i].category, notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
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
