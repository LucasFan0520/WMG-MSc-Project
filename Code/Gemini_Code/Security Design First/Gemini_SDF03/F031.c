// F031.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ImporterRecord;

int main(void) {
    ImporterRecord *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            int n = atoi(line + 6);
            if (n > 0) {
                char **b_names = malloc(n * sizeof(char *));
                char **b_notes = malloc(n * sizeof(char *));
                if (!b_names || !b_notes) exit(1);
                int valid = 1;
                for (int i = 0; i < n; i++) {
                    char *bline = NULL;
                    size_t blen = 0;
                    ssize_t bread = getline(&bline, &blen, stdin);
                    if (bread == -1) {
                        valid = 0;
                        free(bline);
                        break;
                    }
                    if (bread > 0 && bline[bread - 1] == '\n') {
                        bline[bread - 1] = '\0';
                    }
                    char *p = bline;
                    while (*p == ' ') p++;
                    char *name = p;
                    while (*p && *p != ' ') p++;
                    char *note = "";
                    if (*p) {
                        *p = '\0';
                        p++;
                        while (*p == ' ') p++;
                        note = p;
                    }
                    if (strlen(name) == 0 || strlen(note) == 0) {
                        valid = 0;
                    }
                    b_names[i] = strdup(name);
                    b_notes[i] = strdup(note);
                    if (!b_names[i] || !b_notes[i]) exit(1);
                    free(bline);
                }
                if (valid) {
                    for (int i = 0; i < n; i++) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            ImporterRecord *tmp = realloc(arr, capacity * sizeof(ImporterRecord));
                            if (!tmp) exit(1);
                            arr = tmp;
                        }
                        arr[count].name = b_names[i];
                        arr[count].note = b_notes[i];
                        count++;
                    }
                } else {
                    for (int i = 0; i < n; i++) {
                        free(b_names[i]);
                        free(b_notes[i]);
                    }
                }
                free(b_names);
                free(b_notes);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; ) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s\n", arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", arr[i].name, arr[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    free(line);
    return 0;
}
