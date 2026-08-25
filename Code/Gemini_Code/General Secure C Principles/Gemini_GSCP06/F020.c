// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **lines;
    size_t line_count;
    size_t line_capacity;
} Record;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Record *catalog = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread;
        int expecting_title = 1;
        Record current_rec = {NULL, NULL, 0, 0};
        while ((fread = getline(&fline, &flen, f)) != -1) {
            fline[strcspn(fline, "\r\n")] = '\0';
            if (strcmp(fline, "--") == 0) {
                if (current_rec.title) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Record *next = realloc(catalog, capacity * sizeof(Record));
                        if (!next) break;
                        catalog = next;
                    }
                    catalog[count++] = current_rec;
                    current_rec.title = NULL;
                    current_rec.lines = NULL;
                    current_rec.line_count = 0;
                    current_rec.line_capacity = 0;
                }
                expecting_title = 1;
            } else {
                if (expecting_title) {
                    current_rec.title = strdup(fline);
                    expecting_title = 0;
                } else {
                    if (current_rec.line_count >= current_rec.line_capacity) {
                        current_rec.line_capacity = current_rec.line_capacity == 0 ? 4 : current_rec.line_capacity * 2;
                        char **nxt = realloc(current_rec.lines, current_rec.line_capacity * sizeof(char *));
                        if (!nxt) break;
                        current_rec.lines = nxt;
                    }
                    current_rec.lines[current_rec.line_count++] = strdup(fline);
                }
            }
        }
        if (current_rec.title) {
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Record *next = realloc(catalog, capacity * sizeof(Record));
                if (next) catalog = next;
            }
            catalog[count++] = current_rec;
        }
        free(fline);
        fclose(f);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    printf("%s\n", catalog[i].title);
                    for (size_t k = 0; k < catalog[i].line_count; k++) {
                        printf("%s\n", catalog[i].lines[k]);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    for (size_t k = 0; k < catalog[i].line_count; k++) {
                        free(catalog[i].lines[k]);
                    }
                    free(catalog[i].lines);
                    for (size_t j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            Record new_rec;
            new_rec.title = strdup(title);
            new_rec.lines = NULL;
            new_rec.line_count = 0;
            new_rec.line_capacity = 0;
            while ((read = getline(&line, &len, stdin)) != -1) {
                line[strcspn(line, "\r\n")] = '\0';
                if (strcmp(line, "--") == 0) {
                    break;
                }
                if (new_rec.line_count >= new_rec.line_capacity) {
                    new_rec.line_capacity = new_rec.line_capacity == 0 ? 4 : new_rec.line_capacity * 2;
                    char **nxt = realloc(new_rec.lines, new_rec.line_capacity * sizeof(char *));
                    if (!nxt) break;
                    new_rec.lines = nxt;
                }
                new_rec.lines[new_rec.line_count++] = strdup(line);
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Record *next = realloc(catalog, capacity * sizeof(Record));
                if (!next) break;
                catalog = next;
            }
            catalog[count++] = new_rec;
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (size_t k = 0; k < catalog[i].line_count; k++) {
                    printf("%s\n", catalog[i].lines[k]);
                }
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(catalog[i].title);
        for (size_t k = 0; k < catalog[i].line_count; k++) {
            free(catalog[i].lines[k]);
        }
        free(catalog[i].lines);
    }
    free(catalog);
    return 0;
}
