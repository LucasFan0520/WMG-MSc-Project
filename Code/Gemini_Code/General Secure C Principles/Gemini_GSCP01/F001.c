// F001.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main(void) {
    Paragraph *list = NULL;
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
        if (strcmp(cmd, "NEW") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *id = p;
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
            char *text = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    free(list[i].text);
                    list[i].text = strdup(text);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Paragraph *temp = realloc(list, (count + 1) * sizeof(Paragraph));
                if (temp) {
                    list = temp;
                    list[count].id = strdup(id);
                    list[count].text = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *id = p;
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
            char *text = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    size_t old_len = strlen(list[i].text);
                    size_t add_len = strlen(text);
                    char *temp = realloc(list[i].text, old_len + add_len + 1);
                    if (temp) {
                        list[i].text = temp;
                        strcat(list[i].text, text);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *id = p;
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
            char *text = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    free(list[i].text);
                    list[i].text = strdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    free(list[i].id);
                    free(list[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    char *t = list[i].text;
                    while (*t) {
                        if (*t == ' ') {
                            putchar('_');
                        } else {
                            putchar(*t);
                        }
                        t++;
                    }
                    putchar('\n');
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", list[i].id);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].id);
        free(list[i].text);
    }
    free(list);
    free(line);
    return 0;
}
