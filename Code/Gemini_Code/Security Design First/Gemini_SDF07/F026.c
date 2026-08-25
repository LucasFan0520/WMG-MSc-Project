/* F026.c */
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
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            char *cat = NULL;
            char *txt = NULL;
            if (space) {
                *space = '\0';
                cat = args;
                txt = space + 1;
            } else {
                cat = args;
                txt = "";
            }
            char *n_cat = strdup(cat);
            char *n_txt = strdup(txt);
            if (n_cat && n_txt) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Note *tmp = realloc(notes, new_cap * sizeof(Note));
                    if (tmp) {
                        notes = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_cat); free(n_txt);
                        if (space) *space = ' ';
                        continue;
                    }
                }
                notes[count].category = n_cat;
                notes[count].text = n_txt;
                count++;
            } else {
                free(n_cat); free(n_txt);
            }
            if (space) *space = ' ';
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *cat = line + 7;
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
        } else if (strncmp(line, "KEEP ", 5) == 0) {
            char *cat = line + 5;
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
    free(line);
    for (int i = 0; i < count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}
