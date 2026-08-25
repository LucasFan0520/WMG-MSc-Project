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
    FileNote *arr = NULL;
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
        if (strcmp(line, "LOAD") == 0) {
            FILE *f = fopen(argv[1], "r");
            if (f) {
                FileNote *new_arr = NULL;
                size_t new_count = 0;
                size_t new_capacity = 0;
                char *fline = NULL;
                size_t flen = 0;
                ssize_t fread;
                int valid = 1;
                while ((fread = getline(&fline, &flen, f)) != -1) {
                    if (fread > 0 && fline[fread - 1] == '\n') {
                        fline[fread - 1] = '\0';
                    }
                    char *p = fline;
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
                    if (strlen(name) == 0) {
                        valid = 0;
                        break;
                    }
                    if (new_count >= new_capacity) {
                        new_capacity = new_capacity == 0 ? 4 : new_capacity * 2;
                        FileNote *tmp = realloc(new_arr, new_capacity * sizeof(FileNote));
                        if (!tmp) exit(1);
                        new_arr = tmp;
                    }
                    new_arr[new_count].name = strdup(name);
                    new_arr[new_count].note = strdup(note);
                    if (!new_arr[new_count].name || !new_arr[new_count].note) exit(1);
                    new_count++;
                }
                free(fline);
                fclose(f);
                if (valid) {
                    for (size_t i = 0; i < count; i++) {
                        free(arr[i].name);
                        free(arr[i].note);
                    }
                    free(arr);
                    arr = new_arr;
                    count = new_count;
                    capacity = new_capacity;
                } else {
                    for (size_t i = 0; i < new_count; i++) {
                        free(new_arr[i].name);
                        free(new_arr[i].note);
                    }
                    free(new_arr);
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
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
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                FileNote *tmp = realloc(arr, capacity * sizeof(FileNote));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].name = strdup(name);
            arr[count].note = strdup(note);
            if (!arr[count].name || !arr[count].note) exit(1);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
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
