// F020.c
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
    char *title;
    char **body;
    size_t bcount;
} Record;

int main(int argc, char **argv) {
    Record *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *title = read_line(f);
                if (!title) break;
                if (strcmp(title, "--") == 0) {
                    free(title);
                    continue;
                }
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Record));
                }
                arr[count].title = title;
                arr[count].body = NULL;
                arr[count].bcount = 0;
                size_t bcap = 0;
                while (1) {
                    char *bline = read_line(f);
                    if (!bline || strcmp(bline, "--") == 0) {
                        free(bline);
                        break;
                    }
                    if (arr[count].bcount >= bcap) {
                        bcap = bcap == 0 ? 4 : bcap * 2;
                        arr[count].body = realloc(arr[count].body, bcap * sizeof(char *));
                    }
                    arr[count].body[arr[count].bcount++] = bline;
                }
                count++;
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *t = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, t) == 0) {
                    printf("%s\n", arr[i].title);
                    for (size_t j = 0; j < arr[i].bcount; j++) {
                        printf("%s\n", arr[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *t = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, t) == 0) {
                    free(arr[i].title);
                    for (size_t j = 0; j < arr[i].bcount; j++) {
                        free(arr[i].body[j]);
                    }
                    free(arr[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *t = line + 4;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                arr = realloc(arr, cap * sizeof(Record));
            }
            arr[count].title = strdup(t);
            arr[count].body = NULL;
            arr[count].bcount = 0;
            size_t bcap = 0;
            while (1) {
                char *bline = read_line(stdin);
                if (!bline || strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                if (arr[count].bcount >= bcap) {
                    bcap = bcap == 0 ? 4 : bcap * 2;
                    arr[count].body = realloc(arr[count].body, bcap * sizeof(char *));
                }
                arr[count].body[arr[count].bcount++] = bline;
            }
            count++;
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].title);
                for (size_t j = 0; j < arr[i].bcount; j++) {
                    printf("%s\n", arr[i].body[j]);
                }
                printf("--\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        for (size_t j = 0; j < arr[i].bcount; j++) {
            free(arr[i].body[j]);
        }
        free(arr[i].body);
    }
    free(arr);
    return 0;
}
