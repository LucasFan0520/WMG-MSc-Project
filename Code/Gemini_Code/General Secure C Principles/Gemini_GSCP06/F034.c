// F034.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} FileNote;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    char *filepath = argv[1];

    FileNote *store = NULL;
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
        if (strcmp(line, "LOAD") == 0) {
            FILE *f = fopen(filepath, "r");
            if (f) {
                FileNote *temp_store = NULL;
                size_t temp_count = 0;
                size_t temp_capacity = 0;
                int valid = 1;
                char *fline = NULL;
                size_t flen = 0;
                ssize_t fread;
                while ((fread = getline(&fline, &flen, f)) != -1) {
                    fline[strcspn(fline, "\r\n")] = '\0';
                    if (strlen(fline) == 0) continue;
                    char *space = strchr(fline, ' ');
                    if (!space || space == fline || *(space + 1) == '\0') {
                        valid = 0;
                        break;
                    }
                    *space = '\0';
                    char *name = fline;
                    char *note = space + 1;
                    if (temp_count >= temp_capacity) {
                        temp_capacity = temp_capacity == 0 ? 4 : temp_capacity * 2;
                        FileNote *next = realloc(temp_store, temp_capacity * sizeof(FileNote));
                        if (!next) {
                            valid = 0;
                            break;
                        }
                        temp_store = next;
                    }
                    temp_store[temp_count].name = strdup(name);
                    temp_store[temp_count].note = strdup(note);
                    temp_count++;
                }
                free(fline);
                fclose(f);

                if (valid) {
                    for (size_t i = 0; i < count; i++) {
                        free(store[i].name);
                        free(store[i].note);
                    }
                    free(store);
                    store = temp_store;
                    count = temp_count;
                    capacity = temp_capacity;
                } else {
                    for (size_t i = 0; i < temp_count; i++) {
                        free(temp_store[i].name);
                        free(temp_store[i].note);
                    }
                    free(temp_store);
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *note = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    FileNote *next = realloc(store, capacity * sizeof(FileNote));
                    if (!next) break;
                    store = next;
                }
                store[count].name = strdup(name);
                store[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    free(line);
    return 0;
}
