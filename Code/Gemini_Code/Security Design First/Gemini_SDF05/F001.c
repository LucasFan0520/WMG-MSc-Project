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
    int capacity = 0;
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
            char *id_start = line + 4;
            while (*id_start == ' ') id_start++;
            char *space = strchr(id_start, ' ');
            char *text_start = "";
            if (space) {
                *space = '\0';
                text_start = space + 1;
                while (*text_start == ' ') text_start++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *new_text = strdup(text_start);
                if (!new_text) {
                    free(line);
                    for (int i = 0; i < count; i++) { free(notebook[i].id); free(notebook[i].text); }
                    free(notebook);
                    return 1;
                }
                free(notebook[found].text);
                notebook[found].text = new_text;
            } else {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Paragraph *tmp = realloc(notebook, new_cap * sizeof(Paragraph));
                    if (!tmp) {
                        free(line);
                        for (int i = 0; i < count; i++) { free(notebook[i].id); free(notebook[i].text); }
                        free(notebook);
                        return 1;
                    }
                    notebook = tmp;
                    capacity = new_cap;
                }
                char *nid = strdup(id_start);
                char *ntext = strdup(text_start);
                if (!nid || !ntext) {
                    free(nid);
                    free(ntext);
                    free(line);
                    for (int i = 0; i < count; i++) { free(notebook[i].id); free(notebook[i].text); }
                    free(notebook);
                    return 1;
                }
                notebook[count].id = nid;
                notebook[count].text = ntext;
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *id_start = line + 7;
            while (*id_start == ' ') id_start++;
            char *space = strchr(id_start, ' ');
            char *text_start = "";
            if (space) {
                *space = '\0';
                text_start = space + 1;
                while (*text_start == ' ') text_start++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                size_t old_len = strlen(notebook[found].text);
                size_t add_len = strlen(text_start);
                char *new_text = malloc(old_len + add_len + 1);
                if (!new_text) {
                    free(line);
                    for (int i = 0; i < count; i++) { free(notebook[i].id); free(notebook[i].text); }
                    free(notebook);
                    return 1;
                }
                strcpy(new_text, notebook[found].text);
                strcpy(new_text + old_len, text_start);
                free(notebook[found].text);
                notebook[found].text = new_text;
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *id_start = line + 8;
            while (*id_start == ' ') id_start++;
            char *space = strchr(id_start, ' ');
            char *text_start = "";
            if (space) {
                *space = '\0';
                text_start = space + 1;
                while (*text_start == ' ') text_start++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *new_text = strdup(text_start);
                if (!new_text) {
                    free(line);
                    for (int i = 0; i < count; i++) { free(notebook[i].id); free(notebook[i].text); }
                    free(notebook);
                    return 1;
                }
                free(notebook[found].text);
                notebook[found].text = new_text;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id_start = line + 7;
            while (*id_start == ' ') id_start++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id_start) == 0) {
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
        } else if (strcmp(line, "SHOW") == 0 || strncmp(line, "SHOW ", 5) == 0) {
            char *id_start = line + 4;
            if (strncmp(line, "SHOW ", 5) == 0) id_start = line + 5;
            while (*id_start == ' ') id_start++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id_start) == 0) {
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
    free(line);
    for (int i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
