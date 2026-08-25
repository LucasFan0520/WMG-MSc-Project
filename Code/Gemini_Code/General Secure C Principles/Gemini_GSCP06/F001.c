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
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *id = args;
                char *text = space + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    free(notebook[found].text);
                    notebook[found].text = strdup(text);
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Paragraph *next = realloc(notebook, capacity * sizeof(Paragraph));
                        if (!next) {
                            break;
                        }
                        notebook = next;
                    }
                    notebook[count].id = strdup(id);
                    notebook[count].text = strdup(text);
                    count++;
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *id = args;
                char *text = space + 1;
                for (size_t i = 0; i < count; i++) {
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
            char *args = line + 8;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *id = args;
                char *text = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        free(notebook[i].text);
                        notebook[i].text = strdup(text);
                        break;
                    }
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
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *t = notebook[found].text;
                for (size_t i = 0; t[i] != '\0'; i++) {
                    if (t[i] == ' ') {
                        putchar('_');
                    } else {
                        putchar(t[i]);
                    }
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", notebook[i].id);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
