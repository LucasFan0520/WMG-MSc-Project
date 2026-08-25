// F001.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

Paragraph *notebook = NULL;
int count = 0;
int capacity = 0;

int find_p(const char *id) {
    for (int i = 0; i < count; i++) {
        if (notebook[i].id && strcmp(notebook[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *id = space1 + 1;
            char *space2 = strchr(id, ' ');
            if (space2) {
                *space2 = '\0';
            }
            if (strcmp(cmd, "NEW") == 0) {
                if (find_p(id) == -1) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Paragraph *tmp = realloc(notebook, capacity * sizeof(Paragraph));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        notebook = tmp;
                    }
                    notebook[count].id = strdup(id);
                    notebook[count].text = strdup(space2 ? space2 + 1 : "");
                    if (notebook[count].id && notebook[count].text) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                int idx = find_p(id);
                if (idx != -1) {
                    char *t = space2 ? space2 + 1 : "";
                    size_t new_len = strlen(notebook[idx].text) + strlen(t) + 1;
                    char *new_text = realloc(notebook[idx].text, new_len);
                    if (new_text) {
                        notebook[idx].text = new_text;
                        strcat(notebook[idx].text, t);
                    }
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                int idx = find_p(id);
                if (idx != -1) {
                    char *t = space2 ? space2 + 1 : "";
                    char *new_text = strdup(t);
                    if (new_text) {
                        free(notebook[idx].text);
                        notebook[idx].text = new_text;
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int idx = find_p(id);
                if (idx != -1) {
                    free(notebook[idx].id);
                    free(notebook[idx].text);
                    for (int i = idx; i < count - 1; i++) {
                        notebook[i] = notebook[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "SHOW") == 0) {
                int idx = find_p(id);
                if (idx != -1) {
                    char *p = notebook[idx].text;
                    for (size_t i = 0; p[i] != '\0'; i++) {
                        if (p[i] == ' ') {
                            putchar('_');
                        } else {
                            putchar(p[i]);
                        }
                    }
                    putchar('\n');
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", notebook[i].id);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}