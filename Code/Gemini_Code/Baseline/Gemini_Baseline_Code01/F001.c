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
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            char id[256];
            int id_bytes = 0;
            if (sscanf(args, "%255s%n", id, &id_bytes) <= 0) continue;
            char *text = args + id_bytes;
            while (*text == ' ') text++;
            if (strcmp(cmd, "NEW") == 0) {
                notebook = realloc(notebook, (count + 1) * sizeof(Paragraph));
                notebook[count].id = strdup(id);
                notebook[count].text = strdup(text);
                count++;
            } else if (strcmp(cmd, "APPEND") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        int old_len = strlen(notebook[i].text);
                        int text_len = strlen(text);
                        notebook[i].text = realloc(notebook[i].text, old_len + text_len + 1);
                        strcat(notebook[i].text, text);
                        break;
                    }
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        free(notebook[i].text);
                        notebook[i].text = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(args, "%255s", id) > 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        free(notebook[i].id);
                        free(notebook[i].text);
                        for (int j = i; j < count - 1; j++) {
                            notebook[j] = notebook[j + 1];
                        }
                        count--;
                        notebook = realloc(notebook, count * sizeof(Paragraph));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char id[256];
            if (sscanf(args, "%255s", id) > 0) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(notebook[i].id, id) == 0) {
                        found = 1;
                        for (int j = 0; notebook[i].text[j] != '\0'; j++) {
                            if (notebook[i].text[j] == ' ') printf("_");
                            else printf("%c", notebook[i].text[j]);
                        }
                        printf("\n");
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
