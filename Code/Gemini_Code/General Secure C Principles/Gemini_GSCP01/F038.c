// F038.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main(void) {
    RLEString *list = NULL;
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
        if (strcmp(cmd, "STORE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
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
            char *encoded = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            RLEString *temp = realloc(list, (count + 1) * sizeof(RLEString));
            if (temp) {
                list = temp;
                list[count].name = strdup(name);
                list[count].encoded = strdup(encoded);
                count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    char *ep = list[i].encoded;
                    int valid = 1;
                    while (*ep) {
                        if (*ep >= '0' && *ep <= '9') {
                            long long num = 0;
                            while (*ep >= '0' && *ep <= '9') {
                                num = num * 10 + (*ep - '0');
                                if (num > 100000) {
                                    valid = 0;
                                    break;
                                }
                                ep++;
                            }
                            if (!valid || *ep == '\0') {
                                break;
                            }
                            char c = *ep;
                            if (c == ' ') c = '_';
                            for (long long k = 0; k < num; k++) {
                                putchar(c);
                            }
                            ep++;
                        } else {
                            break;
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", list[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].encoded);
    }
    free(list);
    free(line);
    return 0;
}
