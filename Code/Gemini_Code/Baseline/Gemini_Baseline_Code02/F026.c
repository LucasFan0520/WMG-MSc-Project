// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *category;
    char *text;
} Note;

int main() {
    Note *notes = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("[%s] %s\n", notes[i].category, notes[i].text);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                notes = realloc(notes, sizeof(Note) * (count + 1));
                notes[count].category = strdup(args);
                notes[count].text = strdup(p2 + 1);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, args) == 0) {
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
            if (count == 0) { free(notes); notes = NULL; }
            else { notes = realloc(notes, sizeof(Note) * count); }
        } else if (strcmp(cmd, "KEEP") == 0) {
            int i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, args) != 0) {
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
            if (count == 0) { free(notes); notes = NULL; }
            else { notes = realloc(notes, sizeof(Note) * count); }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strstr(notes[i].text, args) != NULL) {
                    printf("[%s] %s\n", notes[i].category, notes[i].text);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}
