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
    Paragraph *arr = NULL;
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
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                text = p;
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
                    Paragraph *tmp = realloc(arr, capacity * sizeof(Paragraph));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].id = strdup(id);
                arr[count].text = strdup(text);
                if (!arr[count].id || !arr[count].text) exit(1);
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                text = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    size_t new_len = strlen(arr[i].text) + strlen(text) + 1;
                    char *tmp = realloc(arr[i].text, new_len);
                    if (!tmp) exit(1);
                    arr[i].text = tmp;
                    strcat(arr[i].text, text);
                    break;
                }
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                text = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].text);
                    arr[i].text = strdup(text);
                    if (!arr[i].text) exit(1);
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    found = 1;
                    char *t = arr[i].text;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].id);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].text);
    }
    free(arr);
    free(line);
    return 0;
}
