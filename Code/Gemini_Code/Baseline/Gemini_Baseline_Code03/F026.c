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
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char cat[256];
            int cat_len = 0;
            if (sscanf(ptr, "%255s%n", cat, &cat_len) > 0) {
                char *text = ptr + cat_len;
                while (*text == ' ') text++;
                notes = realloc(notes, (count + 1) * sizeof(Note));
                notes[count].category = strdup(cat);
                notes[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char cat[256];
            if (sscanf(ptr, "%255s", cat) > 0) {
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
                if (count > 0) {
                    notes = realloc(notes, count * sizeof(Note));
                } else {
                    free(notes);
                    notes = NULL;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char cat[256];
            if (sscanf(ptr, "%255s", cat) > 0) {
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
                if (count > 0) {
                    notes = realloc(notes, count * sizeof(Note));
                } else {
                    free(notes);
                    notes = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char word[256];
            if (sscanf(ptr, "%255s", word) > 0) {
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
