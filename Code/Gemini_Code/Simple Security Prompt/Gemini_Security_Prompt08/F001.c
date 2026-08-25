// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    buf[len] = '\0';
    return buf;
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
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            char *id = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            if (strcmp(cmd, "NEW") == 0) {
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Paragraph *nb = realloc(notebook, cap * sizeof(Paragraph));
                        if (nb) notebook = nb;
                    }
                    if (count < cap) {
                        notebook[count].id = strdup(id);
                        notebook[count].text = strdup(text);
                        count++;
                    }
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        size_t old_len = strlen(notebook[i].text);
                        size_t add_len = strlen(text);
                        char *nt = realloc(notebook[i].text, old_len + add_len + 1);
                        if (nt) {
                            notebook[i].text = nt;
                            strcpy(notebook[i].text + old_len, text);
                        }
                        break;
                    }
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        free(notebook[i].text);
                        notebook[i].text = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = arg1;
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
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = arg1;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = 1;
                    for (size_t j = 0; notebook[i].text[j] != '\0'; j++) {
                        if (notebook[i].text[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(notebook[i].text[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
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
