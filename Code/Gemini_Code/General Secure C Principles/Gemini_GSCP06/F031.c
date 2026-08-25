// F031.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ImportRecord;

int main(void) {
    ImportRecord *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            int n = atoi(line + 6);
            if (n > 0) {
                char **batch_lines = malloc(n * sizeof(char *));
                int all_valid = 1;
                for (int i = 0; i < n; i++) {
                    char *b_line = NULL;
                    size_t b_len = 0;
                    if (getline(&b_line, &b_len, stdin) != -1) {
                        b_line[strcspn(b_line, "\r\n")] = '\0';
                        batch_lines[i] = b_line;
                        char *space = strchr(b_line, ' ');
                        if (!space || space == b_line || *(space + 1) == '\0') {
                            all_valid = 0;
                        }
                    } else {
                        batch_lines[i] = NULL;
                        all_valid = 0;
                    }
                }
                if (all_valid) {
                    for (int i = 0; i < n; i++) {
                        char *space = strchr(batch_lines[i], ' ');
                        *space = '\0';
                        char *name = batch_lines[i];
                        char *note = space + 1;
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            ImportRecord *next = realloc(list, capacity * sizeof(ImportRecord));
                            if (next) list = next;
                        }
                        list[count].name = strdup(name);
                        list[count].note = strdup(note);
                        count++;
                    }
                }
                for (int i = 0; i < n; i++) {
                    free(batch_lines[i]);
                }
                free(batch_lines);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s %s\n", list[i].name, list[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].name, list[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].note);
    }
    free(list);
    free(line);
    return 0;
}
