// F031.c
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
} Record;

int main() {
    Record *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            int n = atoi(line + 6);
            int all_valid = 1;
            Record *batch_arr = malloc(n * sizeof(Record));
            for (int i = 0; i < n; i++) {
                char *bline = read_line(stdin);
                if (!bline) {
                    all_valid = 0;
                    batch_arr[i].name = NULL;
                    batch_arr[i].note = NULL;
                    continue;
                }
                char *space = strchr(bline, ' ');
                if (!space) {
                    all_valid = 0;
                    batch_arr[i].name = NULL;
                    batch_arr[i].note = NULL;
                } else {
                    *space = '\0';
                    batch_arr[i].name = strdup(bline);
                    batch_arr[i].note = strdup(space + 1);
                }
                free(bline);
            }
            if (all_valid) {
                for (int i = 0; i < n; i++) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        arr = realloc(arr, cap * sizeof(Record));
                    }
                    arr[count++] = batch_arr[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    free(batch_arr[i].name);
                    free(batch_arr[i].note);
                }
            }
            free(batch_arr);
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s\n", arr[i].name, arr[i].note);
                    break;
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
