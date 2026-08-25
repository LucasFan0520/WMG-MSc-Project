// F001.c
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "NEW") == 0) {
            char *id = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (id && text) {
                notebook = realloc(notebook, (count + 1) * sizeof(Paragraph));
                notebook[count].id = strdup(id);
                notebook[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *id = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (id && text) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        int old_len = strlen(notebook[i].text);
                        int add_len = strlen(text);
                        notebook[i].text = realloc(notebook[i].text, old_len + add_len + 1);
                        strcat(notebook[i].text, text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *id = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (id && text) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        free(notebook[i].text);
                        notebook[i].text = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = strtok(NULL, " ");
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        free(notebook[i].id);
                        free(notebook[i].text);
                        for (int j = i; j < count - 1; j++) {
                            notebook[j] = notebook[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(notebook);
                    notebook = NULL;
                } else {
                    notebook = realloc(notebook, count * sizeof(Paragraph));
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = strtok(NULL, " ");
            if (id) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        char *t = notebook[i].text;
                        for (int j = 0; t[j]; j++) {
                            if (t[j] == ' ') {
                                putchar('_');
                            } else {
                                putchar(t[j]);
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
            }
        } else if (strcmp(cmd, "LIST") == 0) {
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
    return 0;
}
