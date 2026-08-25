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
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            if (*id == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }

            if (strcmp(cmd, "NEW") == 0) {
                if (found_idx != -1) {
                    char *t = strdup(text);
                    if (t) {
                        free(notebook[found_idx].text);
                        notebook[found_idx].text = t;
                    }
                } else {
                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        Paragraph *new_notebook = realloc(notebook, new_cap * sizeof(Paragraph));
                        if (!new_notebook) continue;
                        notebook = new_notebook;
                        capacity = new_cap;
                    }
                    char *nid = strdup(id);
                    char *ntext = strdup(text);
                    if (nid && ntext) {
                        notebook[count].id = nid;
                        notebook[count].text = ntext;
                        count++;
                    } else {
                        free(nid);
                        free(ntext);
                    }
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                if (found_idx != -1) {
                    size_t old_len = strlen(notebook[found_idx].text);
                    size_t add_len = strlen(text);
                    char *new_text = realloc(notebook[found_idx].text, old_len + add_len + 1);
                    if (new_text) {
                        notebook[found_idx].text = new_text;
                        strcpy(notebook[found_idx].text + old_len, text);
                    }
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                if (found_idx != -1) {
                    char *t = strdup(text);
                    if (t) {
                        free(notebook[found_idx].text);
                        notebook[found_idx].text = t;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*id == '\0') continue;

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
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*id == '\0') continue;

            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    for (size_t j = 0; notebook[i].text[j] != '\0'; j++) {
                        if (notebook[i].text[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(notebook[i].text[j]);
                        }
                    }
                    putchar('\n');
                    found = 1;
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
    }

    for (size_t i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    free(line);
    return 0;
}
