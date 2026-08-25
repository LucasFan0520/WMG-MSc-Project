// F034.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note;

int main(int argc, char **argv) {
    Note *store = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *file_path = (argc > 1) ? argv[1] : NULL;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            if (file_path) {
                FILE *f = fopen(file_path, "r");
                if (f) {
                    Note *new_store = NULL;
                    int new_count = 0;
                    int valid = 1;
                    char *f_line = NULL;
                    size_t f_len = 0;
                    ssize_t f_read;
                    while ((f_read = getline(&f_line, &f_len, f)) != -1) {
                        if (f_read > 0 && f_line[f_read - 1] == '\n') {
                            f_line[f_read - 1] = '\0';
                        }
                        char *space = strchr(f_line, ' ');
                        if (space) {
                            *space = '\0';
                            Note *tmp = realloc(new_store, sizeof(Note) * (new_count + 1));
                            if (tmp) {
                                new_store = tmp;
                                new_store[new_count].name = strdup(f_line);
                                new_store[new_count].note = strdup(space + 1);
                                if (new_store[new_count].name && new_store[new_count].note) {
                                    new_count++;
                                } else {
                                    valid = 0;
                                }
                            } else {
                                valid = 0;
                            }
                        } else {
                            valid = 0;
                        }
                    }
                    free(f_line);
                    fclose(f);
                    if (valid) {
                        for (int i = 0; i < count; i++) {
                            free(store[i].name);
                            free(store[i].note);
                        }
                        free(store);
                        store = new_store;
                        count = new_count;
                    } else {
                        for (int i = 0; i < new_count; i++) {
                            free(new_store[i].name);
                            free(new_store[i].note);
                        }
                        free(new_store);
                    }
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *note = space + 1;
                Note *tmp = realloc(store, sizeof(Note) * (count + 1));
                if (tmp) {
                    store = tmp;
                    store[count].name = strdup(name);
                    store[count].note = strdup(note);
                    if (store[count].name && store[count].note) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(store[idx].name);
                free(store[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    store[i] = store[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    free(line);
    return 0;
}
