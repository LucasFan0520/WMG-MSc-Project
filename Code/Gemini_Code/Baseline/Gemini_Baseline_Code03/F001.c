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
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            char id[256];
            int id_len = 0;
            if (sscanf(ptr, "%255s%n", id, &id_len) <= 0) continue;
            char *text = ptr + id_len;
            while (*text == ' ') text++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (strcmp(cmd, "NEW") == 0) {
                if (found == -1) {
                    notebook = realloc(notebook, (count + 1) * sizeof(Paragraph));
                    notebook[count].id = strdup(id);
                    notebook[count].text = strdup(text);
                    count++;
                } else {
                    free(notebook[found].text);
                    notebook[found].text = strdup(text);
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                if (found != -1) {
                    int old_len = strlen(notebook[found].text);
                    int add_len = strlen(text);
                    notebook[found].text = realloc(notebook[found].text, old_len + add_len + 1);
                    strcat(notebook[found].text, text);
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                if (found != -1) {
                    free(notebook[found].text);
                    notebook[found].text = strdup(text);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(ptr, "%255s", id) > 0) {
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
                    if (count > 0) {
                        notebook = realloc(notebook, count * sizeof(Paragraph));
                    } else {
                        free(notebook);
                        notebook = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char id[256];
            if (sscanf(ptr, "%255s", id) > 0) {
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
