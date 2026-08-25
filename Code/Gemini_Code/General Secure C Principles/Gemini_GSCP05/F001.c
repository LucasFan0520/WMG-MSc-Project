// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main(void) {
    Paragraph *notebook = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *id_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text_start = p + 1;
                while (*text_start == ' ') text_start++;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id_start) == 0) {
                        free(notebook[i].text);
                        notebook[i].text = strdup(text_start);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Paragraph *new_nb = realloc(notebook, capacity * sizeof(Paragraph));
                        if (new_nb) notebook = new_nb;
                    }
                    if (count < capacity) {
                        notebook[count].id = strdup(id_start);
                        notebook[count].text = strdup(text_start);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text_start = p + 1;
                while (*text_start == ' ') text_start++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id_start) == 0) {
                        size_t old_len = strlen(notebook[i].text);
                        size_t add_len = strlen(text_start);
                        char *new_text = realloc(notebook[i].text, old_len + add_len + 1);
                        if (new_text) {
                            notebook[i].text = new_text;
                            strcpy(notebook[i].text + old_len, text_start);
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *id_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text_start = p + 1;
                while (*text_start == ' ') text_start++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id_start) == 0) {
                        free(notebook[i].text);
                        notebook[i].text = strdup(text_start);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id_start = line + 7;
            while (*id_start == ' ') id_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id_start) == 0) {
                    free(notebook[i].id);
                    free(notebook[i].text);
                    for (int j = i; j < count - 1; j++) {
                        notebook[j] = notebook[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *id_start = line + 5;
            while (*id_start == ' ') id_start++;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id_start) == 0) {
                    char *t = notebook[i].text;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
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
