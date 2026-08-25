// F026.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

Note *notes = NULL;
int note_count = 0;
int note_cap = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *text = space2 + 1;
                    if (note_count >= note_cap) {
                        note_cap = note_cap == 0 ? 4 : note_cap * 2;
                        Note *tmp = realloc(notes, note_cap * sizeof(Note));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        notes = tmp;
                    }
                    notes[note_count].category = strdup(arg1);
                    notes[note_count].text = strdup(text);
                    if (notes[note_count].category && notes[note_count].text) {
                        note_count++;
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                for (int i = 0; i < note_count; ) {
                    if (strcmp(notes[i].category, arg1) == 0) {
                        free(notes[i].category);
                        free(notes[i].text);
                        for (int j = i; j < note_count - 1; j++) {
                            notes[j] = notes[j + 1];
                        }
                        note_count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "KEEP") == 0) {
                for (int i = 0; i < note_count; ) {
                    if (strcmp(notes[i].category, arg1) != 0) {
                        free(notes[i].category);
                        free(notes[i].text);
                        for (int j = i; j < note_count - 1; j++) {
                            notes[j] = notes[j + 1];
                        }
                        note_count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                for (int i = 0; i < note_count; i++) {
                    if (strstr(notes[i].text, arg1) != NULL) {
                        printf("%s: %s\n", notes[i].category, notes[i].text);
                    }
                }
            }
        } else {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < note_count; i++) {
                    printf("%s: %s\n", notes[i].category, notes[i].text);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < note_count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}