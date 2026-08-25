// F033.c
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
    char *id;
    char *name;
    char *department;
    char *title;
} Emp;

int main() {
    Emp *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0 || strncmp(line, "UPDATE ", 7) == 0) {
            int is_add = (line[0] == 'A');
            char *p = line + (is_add ? 4 : 7);
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *id = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *name = p2;
                    char *p3 = s2 + 1;
                    char *s3 = strchr(p3, ' ');
                    if (s3) {
                        *s3 = '\0';
                        char *dept = p3;
                        char *title = s3 + 1;
                        if (is_add) {
                            if (count >= cap) {
                                cap = cap == 0 ? 4 : cap * 2;
                                arr = realloc(arr, cap * sizeof(Emp));
                            }
                            arr[count].id = strdup(id);
                            arr[count].name = strdup(name);
                            arr[count].department = strdup(dept);
                            arr[count].title = strdup(title);
                            count++;
                        } else {
                            for (size_t i = 0; i < count; i++) {
                                if (strcmp(arr[i].id, id) == 0) {
                                    free(arr[i].name);
                                    free(arr[i].department);
                                    free(arr[i].title);
                                    arr[i].name = strdup(name);
                                    arr[i].department = strdup(dept);
                                    arr[i].title = strdup(title);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].name);
                    free(arr[i].department);
                    free(arr[i].title);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].department, arr[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].department, arr[i].title);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].name);
        free(arr[i].department);
        free(arr[i].title);
    }
    free(arr);
    return 0;
}
