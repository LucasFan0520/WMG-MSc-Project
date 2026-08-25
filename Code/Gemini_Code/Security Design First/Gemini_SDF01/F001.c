// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main(void) {
    Paragraph *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *text = p;

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (strcmp(cmd, "NEW") == 0) {
                if (found != -1) {
                    free(items[found].text);
                    items[found].text = strdup(text);
                    if (!items[found].text) exit(1);
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Paragraph *new_items = realloc(items, capacity * sizeof(Paragraph));
                        if (!new_items) exit(1);
                        items = new_items;
                    }
                    items[count].id = strdup(id);
                    items[count].text = strdup(text);
                    if (!items[count].id || !items[count].text) exit(1);
                    count++;
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                if (found != -1) {
                    size_t old_len = strlen(items[found].text);
                    size_t add_len = strlen(text);
                    char *new_text = malloc(old_len + add_len + 1);
                    if (!new_text) exit(1);
                    strcpy(new_text, items[found].text);
                    strcat(new_text, text);
                    free(items[found].text);
                    items[found].text = new_text;
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                if (found != -1) {
                    free(items[found].text);
                    items[found].text = strdup(text);
                    if (!items[found].text) exit(1);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].id);
                    free(items[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found != -1) {
                char *txt = items[found].text;
                for (size_t i = 0; txt[i] != '\0'; i++) {
                    if (txt[i] == ' ') putchar('_');
                    else putchar(txt[i]);
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].id);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].id);
        free(items[i].text);
    }
    free(items);
    free(line);
    return 0;
}
