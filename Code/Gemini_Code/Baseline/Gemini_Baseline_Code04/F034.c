// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note;

char *read_line_from_file(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

char *read_line() {
    return read_line_from_file(stdin);
}

int main(int argc, char **argv) {
    Note *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            if (argc > 1) {
                FILE *f = fopen(argv[1], "r");
                if (f) {
                    Note *temp_arr = NULL;
                    int temp_count = 0;
                    int valid = 1;
                    while (1) {
                        char *fline = read_line_from_file(f);
                        if (!fline) break;
                        char *p = fline;
                        while (*p && *p != ' ') p++;
                        if (*p == ' ') {
                            *p = '\0';
                            temp_arr = realloc(temp_arr, (temp_count + 1) * sizeof(Note));
                            temp_arr[temp_count].name = strdup(fline);
                            temp_arr[temp_count].note = strdup(p + 1);
                            temp_count++;
                        } else {
                            valid = 0;
                            free(fline);
                            break;
                        }
                        free(fline);
                    }
                    fclose(f);
                    if (valid) {
                        for (int i = 0; i < count; i++) {
                            free(arr[i].name);
                            free(arr[i].note);
                        }
                        free(arr);
                        arr = temp_arr;
                        count = temp_count;
                    } else {
                        for (int i = 0; i < temp_count; i++) {
                            free(temp_arr[i].name);
                            free(temp_arr[i].note);
                        }
                        free(temp_arr);
                    }
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                arr = realloc(arr, (count + 1) * sizeof(Note));
                arr[count].name = strdup(name);
                arr[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
