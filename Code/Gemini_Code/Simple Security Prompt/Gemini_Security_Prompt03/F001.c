// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Paragraph *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text = p;
            if (strcmp(cmd, "NEW") == 0) {
                list = realloc(list, (count + 1) * sizeof(Paragraph));
                list[count].id = strdup(id);
                list[count].text = strdup(text);
                count++;
            } else if (strcmp(cmd, "APPEND") == 0) {
                for (int i = 0; i < count; i++) {
                    if (list[i].id && strcmp(list[i].id, id) == 0) {
                        size_t old_len = strlen(list[i].text);
                        size_t add_len = strlen(text);
                        list[i].text = realloc(list[i].text, old_len + add_len + 1);
                        strcat(list[i].text, text);
                        break;
                    }
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                for (int i = 0; i < count; i++) {
                    if (list[i].id && strcmp(list[i].id, id) == 0) {
                        free(list[i].text);
                        list[i].text = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].id && strcmp(list[i].id, id) == 0) {
                    free(list[i].id);
                    free(list[i].text);
                    list[i].id = NULL;
                    list[i].text = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (list[i].id && strcmp(list[i].id, id) == 0) {
                    found = 1;
                    for (int j = 0; list[i].text[j]; j++) {
                        putchar(list[i].text[j] == ' ' ? '_' : list[i].text[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].id) {
                    printf("%s\n", list[i].id);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].id) free(list[i].id);
        if (list[i].text) free(list[i].text);
    }
    free(list);
    return 0;
}
