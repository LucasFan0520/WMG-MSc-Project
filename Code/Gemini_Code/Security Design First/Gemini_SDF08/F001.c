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
    Paragraph *notebook = NULL;
    int count = 0;
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
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *id = p;
                char *text = space + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    Paragraph *tmp = realloc(notebook, sizeof(Paragraph) * (count + 1));
                    if (tmp) {
                        notebook = tmp;
                        notebook[count].id = strdup(id);
                        notebook[count].text = strdup(text);
                        if (notebook[count].id && notebook[count].text) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *id = p;
                char *text = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        size_t old_len = strlen(notebook[i].text);
                        size_t add_len = strlen(text);
                        char *new_text = realloc(notebook[i].text, old_len + add_len + 1);
                        if (new_text) {
                            notebook[i].text = new_text;
                            strcpy(notebook[i].text + old_len, text);
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *p = line + 8;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *id = p;
                char *text = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        char *new_text = strdup(text);
                        if (new_text) {
                            free(notebook[i].text);
                            notebook[i].text = new_text;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(notebook[found].id);
                free(notebook[found].text);
                for (int i = found; i < count - 1; i++) {
                    notebook[i] = notebook[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *id = line + 5;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *t = notebook[found].text;
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", notebook[i].id);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    free(line);
    return 0;
}
