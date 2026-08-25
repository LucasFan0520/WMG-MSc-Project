// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main() {
    Paragraph *notebook = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p = line + 4;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        free(notebook[i].text);
                        notebook[i].text = strdup(text);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    notebook = realloc(notebook, (count + 1) * sizeof(Paragraph));
                    notebook[count].id = strdup(id);
                    notebook[count].text = strdup(text);
                    count++;
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        size_t old_len = strlen(notebook[i].text);
                        size_t add_len = strlen(text);
                        notebook[i].text = realloc(notebook[i].text, old_len + add_len + 1);
                        strcpy(notebook[i].text + old_len, text);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *p = line + 8;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        free(notebook[i].text);
                        notebook[i].text = strdup(text);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    free(notebook[i].id);
                    free(notebook[i].text);
                    for (int j = i; j < count - 1; j++) {
                        notebook[j] = notebook[j + 1];
                    }
                    count--;
                    notebook = realloc(notebook, count * sizeof(Paragraph));
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *id = line + 5;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    char *t = notebook[i].text;
                    for (int j = 0; t[j] != '\0'; j++) {
                        if (t[j] == ' ') printf("_");
                        else printf("%c", t[j]);
                    }
                    printf("\n");
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", notebook[i].id);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
