// F033.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

int main(void) {
    Employee *arr = NULL;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            char *name = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                name = p;
            }
            while (*p && *p != ' ') p++;
            char *dept = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                dept = p;
            }
            while (*p && *p != ' ') p++;
            char *title = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                title = p;
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Employee *tmp = realloc(arr, capacity * sizeof(Employee));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].id = strdup(id);
                arr[count].name = strdup(name);
                arr[count].dept = strdup(dept);
                arr[count].title = strdup(title);
                if (!arr[count].id || !arr[count].name || !arr[count].dept || !arr[count].title) exit(1);
                count++;
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            char *name = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                name = p;
            }
            while (*p && *p != ' ') p++;
            char *dept = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                dept = p;
            }
            while (*p && *p != ' ') p++;
            char *title = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                title = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].name);
                    free(arr[i].dept);
                    free(arr[i].title);
                    arr[i].name = strdup(name);
                    arr[i].dept = strdup(dept);
                    arr[i].title = strdup(title);
                    if (!arr[i].name || !arr[i].dept || !arr[i].title) exit(1);
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].name);
                    free(arr[i].dept);
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
            while (*id == ' ') id++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("ID: %s, Name: %s, Dept: %s, Title: %s\n", arr[i].id, arr[i].name, arr[i].dept, arr[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("ID: %s, Name: %s, Dept: %s, Title: %s\n", arr[i].id, arr[i].name, arr[i].dept, arr[i].title);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].name);
        free(arr[i].dept);
        free(arr[i].title);
    }
    free(arr);
    free(line);
    return 0;
}
