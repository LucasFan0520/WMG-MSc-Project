// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *note;
} NoteRecord;

int main(int argc, char **argv) {
    NoteRecord *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    char *file_path = (argc > 1) ? argv[1] : NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            if (file_path) {
                FILE *f = fopen(file_path, "r");
                if (f) {
                    NoteRecord *temp_arr = NULL;
                    size_t temp_count = 0;
                    size_t temp_cap = 0;
                    int valid = 1;
                    while (1) {
                        char *fline = read_line(f);
                        if (!fline) break;
                        char *space = strchr(fline, ' ');
                        if (!space) {
                            valid = 0;
                            free(fline);
                            break;
                        }
                        *space = '\0';
                        if (temp_count >= temp_cap) {
                            temp_cap = temp_cap == 0 ? 4 : temp_cap * 2;
                            temp_arr = realloc(temp_arr, temp_cap * sizeof(NoteRecord));
                        }
                        temp_arr[temp_count].name = strdup(fline);
                        temp_arr[temp_count].note = strdup(space + 1);
                        temp_count++;
                        free(fline);
                    }
                    fclose(f);
                    if (valid) {
                        for (size_t i = 0; i < count; i++) {
                            free(arr[i].name);
                            free(arr[i].note);
                        }
                        free(arr);
                        arr = temp_arr;
                        count = temp_count;
                        cap = temp_cap;
                    } else {
                        for (size_t i = 0; i < temp_count; i++) {
                            free(temp_arr[i].name);
                            free(temp_arr[i].note);
                        }
                        free(temp_arr);
                    }
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *name = line + 4;
            char *space = strchr(name, ' ');
            if (space) {
                *space = '\0';
                char *note = space + 1;
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(NoteRecord));
                }
                arr[count].name = strdup(name);
                arr[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            size_t i = 0;
            while (i < count) {
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
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
