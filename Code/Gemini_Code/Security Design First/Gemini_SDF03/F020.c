// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body_lines;
    size_t line_count;
    size_t line_capacity;
} Record;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Record *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread;
        int in_record = 0;
        while ((fread = getline(&fline, &flen, f)) != -1) {
            if (fread > 0 && fline[fread - 1] == '\n') {
                fline[fread - 1] = '\0';
            }
            if (strcmp(fline, "--") == 0) {
                in_record = 0;
                continue;
            }
            if (!in_record) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Record *tmp = realloc(arr, capacity * sizeof(Record));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].title = strdup(fline);
                arr[count].body_lines = NULL;
                arr[count].line_count = 0;
                arr[count].line_capacity = 0;
                if (!arr[count].title) exit(1);
                in_record = 1;
                count++;
            } else {
                Record *r = &arr[count - 1];
                if (r->line_count >= r->line_capacity) {
                    r->line_capacity = r->line_capacity == 0 ? 4 : r->line_capacity * 2;
                    char **tmp = realloc(r->body_lines, r->line_capacity * sizeof(char *));
                    if (!tmp) exit(1);
                    r->body_lines = tmp;
                }
                r->body_lines[r->line_count] = strdup(fline);
                if (!r->body_lines[r->line_count]) exit(1);
                r->line_count++;
            }
        }
        free(fline);
        fclose(f);
    }
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
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    printf("Title: %s\n", arr[i].title);
                    for (size_t j = 0; j < arr[i].line_count; j++) {
                        printf("%s\n", arr[i].body_lines[j]);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    for (size_t j = 0; j < arr[i].line_count; j++) {
                        free(arr[i].body_lines[j]);
                    }
                    free(arr[i].body_lines);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            while (*title == ' ') title++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Record *tmp = realloc(arr, capacity * sizeof(Record));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].title = strdup(title);
            arr[count].body_lines = NULL;
            arr[count].line_count = 0;
            arr[count].line_capacity = 0;
            if (!arr[count].title) exit(1);
            Record *r = &arr[count];
            count++;
            char *bline = NULL;
            size_t blen = 0;
            ssize_t bread;
            while ((bread = getline(&bline, &blen, stdin)) != -1) {
                if (bread > 0 && bline[bread - 1] == '\n') {
                    bline[bread - 1] = '\0';
                }
                if (strcmp(bline, "--") == 0) {
                    break;
                }
                if (r->line_count >= r->line_capacity) {
                    r->line_capacity = r->line_capacity == 0 ? 4 : r->line_capacity * 2;
                    char **tmp = realloc(r->body_lines, r->line_capacity * sizeof(char *));
                    if (!tmp) exit(1);
                    r->body_lines = tmp;
                }
                r->body_lines[r->line_count] = strdup(bline);
                if (!r->body_lines[r->line_count]) exit(1);
                r->line_count++;
            }
            free(bline);
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].title);
                for (size_t j = 0; j < arr[i].line_count; j++) {
                    printf("%s\n", arr[i].body_lines[j]);
                }
                if (i < count - 1) {
                    printf("--\n");
                }
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        for (size_t j = 0; j < arr[i].line_count; j++) {
            free(arr[i].body_lines[j]);
        }
        free(arr[i].body_lines);
    }
    free(arr);
    free(line);
    return 0;
}
