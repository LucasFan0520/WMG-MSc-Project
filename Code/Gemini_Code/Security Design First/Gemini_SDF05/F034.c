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
    int capacity = 0;
    char *filepath = (argc > 1) ? argv[1] : NULL;

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
        if (strcmp(line, "LOAD") == 0) {
            if (filepath) {
                FILE *f = fopen(filepath, "r");
                if (f) {
                    Note *new_store = NULL;
                    int n_count = 0;
                    int n_capacity = 0;
                    int valid = 1;
                    char *fline = NULL;
                    size_t flen = 0;
                    ssize_t fread;
                    while ((fread = getline(&fline, &flen, f)) != -1) {
                        if (fread > 0 && fline[fread - 1] == '\n') {
                            fline[fread - 1] = '\0';
                        }
                        char *p = fline;
                        while (*p == ' ') p++;
                        if (*p == '\0') continue;
                        char *name = p;
                        char *space = strchr(name, ' ');
                        char *note = "";
                        if (space) {
                            *space = '\0';
                            note = space + 1;
                            while (*note == ' ') note++;
                        }
                        if (n_count >= n_capacity) {
                            int new_cap = n_capacity == 0 ? 4 : n_capacity * 2;
                            Note *tmp = realloc(new_store, new_cap * sizeof(Note));
                            if (!tmp) {
                                valid = 0;
                                break;
                            }
                            new_store = tmp;
                            n_capacity = new_cap;
                        }
                        new_store[n_count].name = strdup(name);
                        new_store[n_count].note = strdup(note);
                        n_count++;
                    }
                    free(fline);
                    fclose(f);
                    if (valid) {
                        for (int i = 0; i < count; i++) {
                            free(store[i].name);
                            free(store[i].note);
                        }
                        free(store);
                        store = new_store;
                        count = n_count;
                        capacity = n_capacity;
                    } else {
                        for (int i = 0; i < n_count; i++) {
                            free(new_store[i].name);
                            free(new_store[i].note);
                        }
                        free(new_store);
                    }
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            char *note = "";
            if (space) {
                *space = '\0';
                note = space + 1;
                while (*note == ' ') note++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Note *tmp = realloc(store, new_cap * sizeof(Note));
                if (!tmp) return 1;
                store = tmp;
                capacity = new_cap;
            }
            store[count].name = strdup(name);
            store[count].note = strdup(note);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(store[found].name);
                free(store[found].note);
                for (int i = found; i < count - 1; i++) {
                    store[i] = store[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", store[i].name, store[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
