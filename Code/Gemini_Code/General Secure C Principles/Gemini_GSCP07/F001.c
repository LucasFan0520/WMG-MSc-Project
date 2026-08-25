// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Paragraph *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
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
                if (strcmp(list[i].id, id) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Paragraph *nl = realloc(list, capacity * sizeof(Paragraph));
                    if (!nl) break;
                    list = nl;
                }
                list[count].id = strdup(id);
                list[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    size_t len1 = strlen(list[i].text);
                    size_t len2 = strlen(text);
                    char *nt = malloc(len1 + len2 + 1);
                    if (nt) {
                        strcpy(nt, list[i].text);
                        strcat(nt, text);
                        free(list[i].text);
                        list[i].text = nt;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    char *nt = strdup(text);
                    if (nt) {
                        free(list[i].text);
                        list[i].text = nt;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    free(list[i].id);
                    free(list[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    found = 1;
                    for (size_t j = 0; list[i].text[j]; j++) {
                        if (list[i].text[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(list[i].text[j]);
                        }
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
                printf("%s\n", list[i].id);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].id);
        free(list[i].text);
    }
    free(list);
    return 0;
}
