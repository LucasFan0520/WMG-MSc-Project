// F034.c
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
    char *name;
    char *note;
} Note;

int main(int argc, char **argv) {
    Note *notes = NULL;
    int count = 0;
    char *filepath = (argc > 1) ? argv[1] : NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LOAD") == 0 && filepath) {
                FILE *f = fopen(filepath, "r");
                if (f) {
                    Note *temp_notes = NULL;
                    int temp_count = 0;
                    int valid = 1;
                    while (1) {
                        char *fline = read_line(f);
                        if (!fline) break;
                        char *sp = strchr(fline, ' ');
                        if (!sp) {
                            valid = 0;
                            free(fline);
                            break;
                        }
                        *sp = '\0';
                        temp_notes = realloc(temp_notes, sizeof(Note) * (temp_count + 1));
                        temp_notes[temp_count].name = strdup(fline);
                        temp_notes[temp_count].note = strdup(sp + 1);
                        temp_count++;
                        free(fline);
                    }
                    fclose(f);
                    if (valid) {
                        for (int i = 0; i < count; i++) {
                            free(notes[i].name); free(notes[i].note);
                        }
                        free(notes);
                        notes = temp_notes;
                        count = temp_count;
                    } else {
                        for (int i = 0; i < temp_count; i++) {
                            free(temp_notes[i].name); free(temp_notes[i].note);
                        }
                        free(temp_notes);
                    }
                }
            } else if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", notes[i].name, notes[i].note);
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
                notes[count].name = strdup(args);
                notes[count].note = strdup(p2 + 1);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(notes[i].name, args) == 0) {
                    free(notes[i].name); free(notes[i].note);
                    for (int j = i; j < count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    count--;
                    if (count == 0) { free(notes); notes = NULL; }
                    else { notes = realloc(notes, sizeof(Note) * count); }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(notes[i].name); free(notes[i].note);
    }
    free(notes);
    return 0;
}
