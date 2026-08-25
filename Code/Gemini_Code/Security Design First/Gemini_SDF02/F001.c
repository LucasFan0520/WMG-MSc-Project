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
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    
    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;
        char *id = p;
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
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Paragraph *temp = realloc(notebook, new_cap * sizeof(Paragraph));
                    if (!temp) {
                        break;
                    }
                    notebook = temp;
                    capacity = new_cap;
                }
                notebook[count].id = strdup(id);
                notebook[count].text = strdup(text);
                if (notebook[count].id && notebook[count].text) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    size_t old_len = strlen(notebook[i].text);
                    size_t add_len = strlen(text);
                    char *temp = realloc(notebook[i].text, old_len + add_len + 1);
                    if (temp) {
                        notebook[i].text = temp;
                        strcpy(notebook[i].text + old_len, text);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    char *temp = strdup(text);
                    if (temp) {
                        free(notebook[i].text);
                        notebook[i].text = temp;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
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
        } else if (strcmp(cmd, "LIST") == 0) {
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
