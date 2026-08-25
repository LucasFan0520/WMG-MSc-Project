// F002.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} MessageRecord;

int main(void) {
    MessageRecord *list = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *tag = p;
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
            char *msg = p;
            MessageRecord *temp = realloc(list, (count + 1) * sizeof(MessageRecord));
            if (temp) {
                list = temp;
                list[count].tag = strdup(tag);
                list[count].message = strdup(msg);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *tag1 = p;
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
            char *tag2 = p;
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
            char *newtag = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!m1 && strcmp(list[i].tag, tag1) == 0) {
                    m1 = list[i].message;
                }
                if (!m2 && strcmp(list[i].tag, tag2) == 0) {
                    m2 = list[i].message;
                }
            }
            if (m1 && m2) {
                char *new_msg = malloc(strlen(m1) + strlen(m2) + 2);
                if (new_msg) {
                    sprintf(new_msg, "%s %s", m1, m2);
                    MessageRecord *temp = realloc(list, (count + 1) * sizeof(MessageRecord));
                    if (temp) {
                        list = temp;
                        list[count].tag = strdup(newtag);
                        list[count].message = new_msg;
                        count++;
                    } else {
                        free(new_msg);
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].tag, tag) == 0) {
                    free(list[i].tag);
                    free(list[i].message);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].tag, tag) == 0) {
                    char *t = list[i].message;
                    while (*t) {
                        if (*t == ' ') {
                            putchar('_');
                        } else {
                            putchar(*t);
                        }
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].tag, list[i].message);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].tag);
        free(list[i].message);
    }
    free(list);
    free(line);
    return 0;
}
