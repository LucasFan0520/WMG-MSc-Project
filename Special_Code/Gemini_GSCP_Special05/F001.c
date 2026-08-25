// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main(void) {
    Paragraph *notebook = NULL;
    size_t count = 0;
    size_t cap = 0;
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
            char *text = "";
            if (*p == ' ') {
                *p = '\0';
                text = p + 1;
            }
            size_t found = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != (size_t)-1) {
                free(notebook[found].text);
                notebook[found].text = safe_dup(text);
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Paragraph *nb = realloc(notebook, cap * sizeof(Paragraph));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    notebook = nb;
                }
                notebook[count].id = safe_dup(id);
                notebook[count].text = safe_dup(text);
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            char *id = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p == ' ') {
                *p = '\0';
                text = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    size_t old_len = strlen(notebook[i].text);
                    size_t add_len = strlen(text);
                    char *nt = malloc(old_len + add_len + 1);
                    if (nt) {
                        strcpy(nt, notebook[i].text);
                        strcat(nt, text);
                        free(notebook[i].text);
                        notebook[i].text = nt;
                    }
                    break;
                }
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *p = line + 8;
            char *id = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p == ' ') {
                *p = '\0';
                text = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    free(notebook[i].text);
                    notebook[i].text = safe_dup(text);
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    free(notebook[i].id);
                    free(notebook[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        notebook[j] = notebook[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *id = line + 5;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = 1;
                    char *t = notebook[i].text;
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
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", notebook[i].id);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
