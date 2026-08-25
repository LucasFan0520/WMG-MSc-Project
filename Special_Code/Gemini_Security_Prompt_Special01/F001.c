// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main(void) {
    Paragraph *notebook = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (notebook[i].id) {
                    printf("%s\n", notebook[i].id);
                }
            }
        } else if (strcmp(cmd, "NEW") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            char *text = id_end;
            if (*id_end != '\0') {
                *id_end = '\0';
                text++;
            }
            while (*text == ' ') text++;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (notebook[i].id && strcmp(notebook[i].id, id) == 0) {
                    free(notebook[i].text);
                    notebook[i].text = mystrdup(text);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Paragraph *new_notebook = realloc(notebook, capacity * sizeof(Paragraph));
                    if (!new_notebook) {
                        free(line);
                        break;
                    }
                    notebook = new_notebook;
                }
                notebook[count].id = mystrdup(id);
                notebook[count].text = mystrdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            char *text = id_end;
            if (*id_end != '\0') {
                *id_end = '\0';
                text++;
            }
            while (*text == ' ') text++;
            for (size_t i = 0; i < count; i++) {
                if (notebook[i].id && strcmp(notebook[i].id, id) == 0) {
                    size_t old_len = strlen(notebook[i].text);
                    size_t add_len = strlen(text);
                    char *new_text = malloc(old_len + add_len + 1);
                    if (new_text) {
                        strcpy(new_text, notebook[i].text);
                        strcat(new_text, text);
                        free(notebook[i].text);
                        notebook[i].text = new_text;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            char *text = id_end;
            if (*id_end != '\0') {
                *id_end = '\0';
                text++;
            }
            while (*text == ' ') text++;
            for (size_t i = 0; i < count; i++) {
                if (notebook[i].id && strcmp(notebook[i].id, id) == 0) {
                    free(notebook[i].text);
                    notebook[i].text = mystrdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            *id_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (notebook[i].id && strcmp(notebook[i].id, id) == 0) {
                    free(notebook[i].id);
                    free(notebook[i].text);
                    notebook[i].id = NULL;
                    notebook[i].text = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            *id_end = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (notebook[i].id && strcmp(notebook[i].id, id) == 0) {
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
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        if (notebook[i].id) free(notebook[i].id);
        if (notebook[i].text) free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
