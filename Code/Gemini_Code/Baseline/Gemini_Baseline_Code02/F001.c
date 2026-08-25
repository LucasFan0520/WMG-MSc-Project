// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *id;
    char *text;
} Item;

int main() {
    Item *items = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", items[i].id);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *id = p1 + 1;
        char *p2 = strchr(id, ' ');
        char *text = NULL;
        if (p2) {
            *p2 = '\0';
            text = p2 + 1;
        }
        if (strcmp(cmd, "NEW") == 0 && text) {
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) { found = 1; break; }
            }
            if (!found) {
                items = realloc(items, sizeof(Item) * (count + 1));
                items[count].id = strdup(id);
                items[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0 && text) {
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    size_t new_len = strlen(items[i].text) + strlen(text) + 1;
                    items[i].text = realloc(items[i].text, new_len);
                    strcat(items[i].text, text);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0 && text) {
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].text);
                    items[i].text = strdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].id);
                    free(items[i].text);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    if (count == 0) { free(items); items = NULL; }
                    else { items = realloc(items, sizeof(Item) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    found = 1;
                    for (int j = 0; items[i].text[j]; j++) {
                        if (items[i].text[j] == ' ') printf("_");
                        else printf("%c", items[i].text[j]);
                    }
                    printf("\n");
                    break;
                }
            }
            if (!found) printf("NOT_FOUND\n");
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].id);
        free(items[i].text);
    }
    free(items);
    return 0;
}
