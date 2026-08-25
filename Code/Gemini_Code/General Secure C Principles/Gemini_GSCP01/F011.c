// F011.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *list = NULL;
    size_t count = 0;
    char *discarded_title = NULL;
    char *discarded_body = NULL;
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
        if (strcmp(cmd, "CREATE") == 0) {
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
            char *body = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    free(list[i].body);
                    list[i].body = strdup(body);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Draft *temp = realloc(list, (count + 1) * sizeof(Draft));
                if (temp) {
                    list = temp;
                    list[count].title = strdup(title);
                    list[count].body = strdup(body);
                    count++;
                }
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
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
            char *body = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    free(list[i].body);
                    list[i].body = strdup(body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
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
                    free(discarded_title);
                    free(discarded_body);
                    discarded_title = list[i].title;
                    discarded_body = list[i].body;
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (discarded_title) {
                Draft *temp = realloc(list, (count + 1) * sizeof(Draft));
                if (temp) {
                    list = temp;
                    list[count].title = discarded_title;
                    list[count].body = discarded_body;
                    count++;
                    discarded_title = NULL;
                    discarded_body = NULL;
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
                    printf("%s %s\n", list[i].title, list[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].title, list[i].body);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].body);
    }
    free(list);
    free(discarded_title);
    free(discarded_body);
    free(line);
    return 0;
}
