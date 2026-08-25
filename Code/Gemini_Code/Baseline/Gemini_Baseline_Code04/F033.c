// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
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

int main() {
    Employee *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *name = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    p++;
                    char *dept = p;
                    while (*p && *p != ' ') p++;
                    if (*p == ' ') {
                        *p = '\0';
                        char *title = p + 1;
                        arr = realloc(arr, (count + 1) * sizeof(Employee));
                        arr[count].id = strdup(id);
                        arr[count].name = strdup(name);
                        arr[count].department = strdup(dept);
                        arr[count].title = strdup(title);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p = line + 7;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *name = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    p++;
                    char *dept = p;
                    while (*p && *p != ' ') p++;
                    if (*p == ' ') {
                        *p = '\0';
                        char *title = p + 1;
                        for (int i = 0; i < count; i++) {
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].name);
                    free(arr[i].department);
                    free(arr[i].title);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].department, arr[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].department, arr[i].title);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].name);
        free(arr[i].department);
        free(arr[i].title);
    }
    free(arr);
    return 0;
}
