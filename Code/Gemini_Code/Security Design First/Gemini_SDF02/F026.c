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
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Note *temp = realloc(notes, new_cap * sizeof(Note));
                if (!temp) break;
                notes = temp;
                capacity = new_cap;
            }
            notes[count].category = strdup(cat);
            notes[count].text = strdup(text);
            if (notes[count].category && notes[count].text) count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = p;
            for (size_t i = 0; i < count; ) {
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
            char *cat = p;
            for (size_t i = 0; i < count; ) {
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
            char *word = p;
            for (size_t i = 0; i < count; i++) {
                if (strstr(notes[i].text, word) != NULL) {
                    printf("%s %s\n", notes[i].category, notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}
