// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

int main(void) {
    Note *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text = p;
            list = realloc(list, (count + 1) * sizeof(Note));
            list[count].category = strdup(cat);
            list[count].text = strdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].category && strcmp(list[i].category, cat) == 0) {
                    free(list[i].category);
                    free(list[i].text);
                    list[i].category = NULL;
                    list[i].text = NULL;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            while (*p == ' ') p++;
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].category && strcmp(list[i].category, cat) != 0) {
                    free(list[i].category);
                    free(list[i].text);
                    list[i].category = NULL;
                    list[i].text = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *word = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].text && strstr(list[i].text, word) != NULL) {
                    printf("[%s] %s\n", list[i].category, list[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].category) {
                    printf("[%s] %s\n", list[i].category, list[i].text);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].category) {
            free(list[i].category);
            free(list[i].text);
        }
    }
    free(list);
    return 0;
}
