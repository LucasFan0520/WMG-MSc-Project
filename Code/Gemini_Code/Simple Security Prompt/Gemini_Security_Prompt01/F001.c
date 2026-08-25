// F001.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main() {
    Paragraph *notebook = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "NEW") == 0 && args) {
            char *id = args;
            char *space2 = strchr(args, ' ');
            char *text = "";
            if (space2) {
                *space2 = '\0';
                text = space2 + 1;
                while (*text == ' ') text++;
            }
            notebook = realloc(notebook, (count + 1) * sizeof(Paragraph));
            if (!notebook) return 1;
            notebook[count].id = strdup(id);
            notebook[count].text = strdup(text);
            count++;
        } else if (strcmp(cmd, "APPEND") == 0 && args) {
            char *id = args;
            char *space2 = strchr(args, ' ');
            char *text = "";
            if (space2) {
                *space2 = '\0';
                text = space2 + 1;
                while (*text == ' ') text++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    size_t new_len = strlen(notebook[i].text) + strlen(text) + 1;
                    char *new_text = realloc(notebook[i].text, new_len);
                    if (!new_text) return 1;
                    strcat(new_text, text);
                    notebook[i].text = new_text;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0 && args) {
            char *id = args;
            char *space2 = strchr(args, ' ');
            char *text = "";
            if (space2) {
                *space2 = '\0';
                text = space2 + 1;
                while (*text == ' ') text++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    free(notebook[i].text);
                    notebook[i].text = strdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, args) == 0) {
                    free(notebook[i].id);
                    free(notebook[i].text);
                    for (int j = i; j < count - 1; j++) {
                        notebook[j] = notebook[j + 1];
                    }
                    count--;
                    notebook = realloc(notebook, count * sizeof(Paragraph));
                    if (count > 0 && !notebook) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0 && args) {
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, args) == 0) {
                    found = 1;
                    for (int j = 0; notebook[i].text[j] != '\0'; j++) {
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
