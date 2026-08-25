/* F001.c */
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
    int capacity = 0;
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
            char *id = NULL;
            char *text = NULL;
            if (space) {
                *space = '\0';
                id = strdup(args);
                text = strdup(space + 1);
                *space = ' ';
            } else {
                id = strdup(args);
                text = strdup("");
            }
            if (!id || !text) {
                free(id);
                free(text);
                continue;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Paragraph *tmp = realloc(notebook, new_cap * sizeof(Paragraph));
                if (!tmp) {
                    free(id);
                    free(text);
                    continue;
                }
                notebook = tmp;
                capacity = new_cap;
            }
            notebook[count].id = id;
            notebook[count].text = text;
            count++;
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            char *id = NULL;
            char *text = NULL;
            if (space) {
                *space = '\0';
                id = args;
                text = space + 1;
            } else {
                id = args;
                text = "";
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    size_t old_len = strlen(notebook[i].text);
                    size_t add_len = strlen(text);
                    char *new_text = realloc(notebook[i].text, old_len + add_len + 1);
                    if (new_text) {
                        strcpy(new_text + old_len, text);
                        notebook[i].text = new_text;
                    }
                    break;
                }
            }
            if (space) *space = ' ';
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *args = line + 8;
            char *space = strchr(args, ' ');
            char *id = NULL;
            char *text = NULL;
            if (space) {
                *space = '\0';
                id = args;
                text = space + 1;
            } else {
                id = args;
                text = "";
            }
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
            if (space) *space = ' ';
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
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *id = line + 5;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = 1;
                    char *p = notebook[i].text;
                    while (*p) {
                        if (*p == ' ') {
                            putchar('_');
                        } else {
                            putchar(*p);
                        }
                        p++;
                    }
                    putchar('\n');
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
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
