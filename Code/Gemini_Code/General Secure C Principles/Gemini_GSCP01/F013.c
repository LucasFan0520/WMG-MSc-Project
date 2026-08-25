// F013.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    char *status;
} Task;

int main(void) {
    Task *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "TODO") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *note = p;
            Task *temp = realloc(list, (count + 1) * sizeof(Task));
            if (temp) {
                list = temp;
                list[count].title = strdup(title);
                list[count].note = strdup(note);
                list[count].status = strdup("TODO");
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0 && strcmp(list[i].status, "TODO") == 0) {
                    free(list[i].status);
                    list[i].status = strdup("DOING");
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0 && (strcmp(list[i].status, "DOING") == 0 || strcmp(list[i].status, "TODO") == 0)) {
                    free(list[i].status);
                    list[i].status = strdup("DONE");
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    free(list[i].title);
                    free(list[i].note);
                    free(list[i].status);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    printf("%s %s\n", list[i].status, list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("--- TODO ---\n");
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].status, "TODO") == 0) {
                    printf("%s: %s\n", list[i].title, list[i].note);
                }
            }
            printf("--- DOING ---\n");
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].status, "DOING") == 0) {
                    printf("%s: %s\n", list[i].title, list[i].note);
                }
            }
            printf("--- DONE ---\n");
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].status, "DONE") == 0) {
                    printf("%s: %s\n", list[i].title, list[i].note);
                }
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].note);
        free(list[i].status);
    }
    free(list);
    free(line);
    return 0;
}
