// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

Note *notes = NULL;
int n_count = 0;

int main() {
    char cmd[32];
    char category[1024];
    char text[10240];
    char word[1024];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", category);
            fgets(text, sizeof(text), stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            text[strcspn(text, "\n")] = '\0';
            notes = realloc(notes, sizeof(Note) * (n_count + 1));
            notes[n_count].category = strdup(category);
            notes[n_count].text = strdup(text);
            n_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", category);
            for (int i = 0; i < n_count; ) {
                if (strcmp(notes[i].category, category) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < n_count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    n_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            scanf("%s", category);
            for (int i = 0; i < n_count; ) {
                if (strcmp(notes[i].category, category) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < n_count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    n_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", word);
            for (int i = 0; i < n_count; i++) {
                if (strstr(notes[i].text, word) != NULL) {
                    printf("%s %s\n", notes[i].category, notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < n_count; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }

    for (int i = 0; i < n_count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);

    return 0;
}