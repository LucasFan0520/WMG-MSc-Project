// F005.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char **lines = NULL;
    int count = 0;
    int capacity = 0;
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
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (p == endptr || idx < 0 || idx > count) continue;
            while (*endptr == ' ') endptr++;
            char *text = endptr;
            
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                char **tmp = realloc(lines, new_cap * sizeof(char *));
                if (!tmp) {
                    free(line);
                    for(int i=0; i<count; i++) free(lines[i]);
                    free(lines);
                    return 1;
                }
                lines = tmp;
                capacity = new_cap;
            }
            char *nt = strdup(text);
            if (!nt) {
                free(line);
                for(int i=0; i<count; i++) free(lines[i]);
                free(lines);
                return 1;
            }
            for (int i = count; i > idx; i--) {
                lines[i] = lines[i - 1];
            }
            lines[idx] = nt;
            count++;
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            while (*text == ' ') text++;
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                char **tmp = realloc(lines, new_cap * sizeof(char *));
                if (!tmp) {
                    free(line);
                    for(int i=0; i<count; i++) free(lines[i]);
                    free(lines);
                    return 1;
                }
                lines = tmp;
                capacity = new_cap;
            }
            char *nt = strdup(text);
            if (!nt) {
                free(line);
                for(int i=0; i<count; i++) free(lines[i]);
                free(lines);
                return 1;
            }
            lines[count] = nt;
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (p == endptr || idx < 0 || idx >= count) continue;
            free(lines[idx]);
            for (int i = idx; i < count - 1; i++) {
                lines[i] = lines[i + 1];
            }
            count--;
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (p == endptr || idx < 0 || idx >= count) continue;
            while (*endptr == ' ') endptr++;
            char *text = endptr;
            char *nt = strdup(text);
            if (!nt) {
                free(line);
                for(int i=0; i<count; i++) free(lines[i]);
                free(lines);
                return 1;
            }
            free(lines[idx]);
            lines[idx] = nt;
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *t = lines[i];
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
