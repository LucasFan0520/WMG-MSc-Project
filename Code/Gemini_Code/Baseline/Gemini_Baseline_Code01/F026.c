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
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char cat[256];
            int cat_bytes = 0;
            if (sscanf(args, "%255s%n", cat, &cat_bytes) == 1) {
                char *text = args + cat_bytes;
                while (*text == ' ') text++;
                notes = realloc(notes, (count + 1) * sizeof(Note));
                notes[count].category = strdup(cat);
                notes[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char cat[256];
            if (sscanf(args, "%255s", cat) == 1) {
                int i = 0;
                while (i < count) {
                    if (strcmp(notes[i].category, cat) == 0) {
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
                notes = realloc(notes, count * sizeof(Note));
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char cat[256];
            if (sscanf(args, "%255s", cat) == 1) {
                int i = 0;
                while (i < count) {
                    if (strcmp(notes[i].category, cat) != 0) {
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
                notes = realloc(notes, count * sizeof(Note));
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char word[256];
            if (sscanf(args, "%255s", word) == 1) {
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
