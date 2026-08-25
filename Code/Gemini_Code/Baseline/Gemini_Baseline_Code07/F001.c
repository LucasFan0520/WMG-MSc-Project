// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main() {
    Paragraph *notebook = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", notebook[i].id);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *id_start = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        int id_len = p - id_start;
        char *id = malloc(id_len + 1);
        memcpy(id, id_start, id_len);
        id[id_len] = '\0';
        while (*p == ' ' || *p == '\t') p++;
        char *text = p;
        if (strcmp(cmd, "NEW") == 0) {
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(notebook[found].text);
                notebook[found].text = strdup(text);
                free(id);
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    notebook = realloc(notebook, capacity * sizeof(Paragraph));
                }
                notebook[count].id = id;
                notebook[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                size_t old_len = strlen(notebook[found].text);
                size_t add_len = strlen(text);
                notebook[found].text = realloc(notebook[found].text, old_len + add_len + 1);
                strcpy(notebook[found].text + old_len, text);
            }
            free(id);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(notebook[found].text);
                notebook[found].text = strdup(text);
            }
            free(id);
        } else if (strcmp(cmd, "DELETE") == 0) {
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
            }
            free(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                for (int i = 0; notebook[found].text[i]; i++) {
                    if (notebook[found].text[i] == ' ') {
                        printf("_");
                    } else {
                        printf("%c", notebook[found].text[i]);
                    }
                }
                printf("\n");
            } else {
                printf("NOT_FOUND\n");
            }
            free(id);
        } else {
            free(id);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
