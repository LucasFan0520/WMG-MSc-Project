// F013.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { STATUS_TODO, STATUS_DOING, STATUS_DONE } TaskStatus;

typedef struct {
    char *title;
    char *note;
    TaskStatus status;
} Task;

int main(void) {
    Task *arr = NULL;
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
        if (strncmp(line, "TODO ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
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
                Task *tmp = realloc(arr, capacity * sizeof(Task));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].title = strdup(title);
            arr[count].note = strdup(note);
            arr[count].status = STATUS_TODO;
            if (!arr[count].title || !arr[count].note) exit(1);
            count++;
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title = line + 6;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (arr[i].status == STATUS_TODO && strcmp(arr[i].title, title) == 0) {
                    arr[i].status = STATUS_DOING;
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (arr[i].status == STATUS_DOING && strcmp(arr[i].title, title) == 0) {
                    arr[i].status = STATUS_DONE;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    const char *st_str = "TODO";
                    if (arr[i].status == STATUS_DOING) st_str = "DOING";
                    else if (arr[i].status == STATUS_DONE) st_str = "DONE";
                    printf("%s %s\n", st_str, arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            printf("TODO:\n");
            for (size_t i = 0; i < count; i++) {
                if (arr[i].status == STATUS_TODO) printf("%s %s\n", arr[i].title, arr[i].note);
            }
            printf("DOING:\n");
            for (size_t i = 0; i < count; i++) {
                if (arr[i].status == STATUS_DOING) printf("%s %s\n", arr[i].title, arr[i].note);
            }
            printf("DONE:\n");
            for (size_t i = 0; i < count; i++) {
                if (arr[i].status == STATUS_DONE) printf("%s %s\n", arr[i].title, arr[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].note);
    }
    free(arr);
    free(line);
    return 0;
}
