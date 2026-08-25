// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *line;
} BodyLine;

typedef struct {
    char *title;
    BodyLine *body;
    int body_count;
} CatalogRecord;

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
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    CatalogRecord *arr = NULL;
    int count = 0;
    if (f) {
        while (1) {
            char *title = read_line_from_file(f);
            if (!title) break;
            if (strcmp(title, "--") == 0) {
                free(title);
                continue;
            }
            arr = realloc(arr, (count + 1) * sizeof(CatalogRecord));
            arr[count].title = title;
            arr[count].body = NULL;
            arr[count].body_count = 0;
            while (1) {
                char *bline = read_line_from_file(f);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                int bc = arr[count].body_count;
                arr[count].body = realloc(arr[count].body, (bc + 1) * sizeof(BodyLine));
                arr[count].body[bc].line = bline;
                arr[count].body_count++;
            }
            count++;
        }
        fclose(f);
    }
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    printf("%s\n", arr[i].title);
                    for (int j = 0; j < arr[i].body_count; j++) {
                        printf("%s\n", arr[i].body[j].line);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    for (int j = 0; j < arr[i].body_count; j++) {
                        free(arr[i].body[j].line);
                    }
                    free(arr[i].body);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            arr = realloc(arr, (count + 1) * sizeof(CatalogRecord));
            arr[count].title = strdup(title);
            arr[count].body = NULL;
            arr[count].body_count = 0;
            while (1) {
                char *bline = read_line();
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                int bc = arr[count].body_count;
                arr[count].body = realloc(arr[count].body, (bc + 1) * sizeof(BodyLine));
                arr[count].body[bc].line = bline;
                arr[count].body_count++;
            }
            count++;
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].title);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        for (int j = 0; j < arr[i].body_count; j++) {
            free(arr[i].body[j].line);
        }
        free(arr[i].body);
    }
    free(arr);
    return 0;
}
