// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Paragraph *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", list[i].id);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *id = rest;
                char *text = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].id, id) == 0) {
                        found = i;
                        break;
                    }
                }
                if (strcmp(cmd, "NEW") == 0 && found == -1) {
                    list = realloc(list, (count + 1) * sizeof(Paragraph));
                    list[count].id = strdup(id);
                    list[count].text = strdup(text);
                    count++;
                } else if (strcmp(cmd, "APPEND") == 0 && found != -1) {
                    size_t old_len = strlen(list[found].text);
                    size_t add_len = strlen(text);
                    list[found].text = realloc(list[found].text, old_len + add_len + 1);
                    strcat(list[found].text, text);
                } else if (strcmp(cmd, "REPLACE") == 0 && found != -1) {
                    free(list[found].text);
                    list[found].text = strdup(text);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].id);
                free(list[found].text);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Paragraph));
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                for (int i = 0; list[found].text[i] != '\0'; i++) {
                    if (list[found].text[i] == ' ') {
                        putchar('_');
                    } else {
                        putchar(list[found].text[i]);
                    }
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].id);
        free(list[i].text);
    }
    free(list);
    return 0;
}