// F001.c
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
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *id = line + 4;
            while (*id == ' ') {
                id++;
            }
            char *sp = strchr(id, ' ');
            char *text = "";
            if (sp) {
                *sp = '\0';
                text = sp + 1;
                while (*text == ' ') {
                    text++;
                }
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                Paragraph *tmp = realloc(notebook, (count + 1) * sizeof(Paragraph));
                if (tmp) {
                    notebook = tmp;
                    notebook[count].id = strdup(id);
                    notebook[count].text = strdup(text);
                    count++;
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') {
                id++;
            }
            char *sp = strchr(id, ' ');
            char *text = "";
            if (sp) {
                *sp = '\0';
                text = sp + 1;
                while (*text == ' ') {
                    text++;
                }
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    size_t old_len = strlen(notebook[i].text);
                    size_t new_len = old_len + strlen(text) + 1;
                    char *new_text = realloc(notebook[i].text, new_len);
                    if (new_text) {
                        notebook[i].text = new_text;
                        strcpy(notebook[i].text + old_len, text);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *id = line + 8;
            while (*id == ' ') {
                id++;
            }
            char *sp = strchr(id, ' ');
            char *text = "";
            if (sp) {
                *sp = '\0';
                text = sp + 1;
                while (*text == ' ') {
                    text++;
                }
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') {
                id++;
            }
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
            while (*id == ' ') {
                id++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                for (int k = 0; notebook[found].text[k]; k++) {
                    putchar(notebook[found].text[k] == ' ' ? '_' : notebook[found].text[k]);
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
    free(line);
    for (int i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
