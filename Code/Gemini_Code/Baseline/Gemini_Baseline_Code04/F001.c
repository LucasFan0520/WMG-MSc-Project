// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

void print_underscores(const char *s) {
    while (*s) {
        if (*s == ' ') {
            putchar('_');
        } else {
            putchar(*s);
        }
        s++;
    }
    putchar('\n');
}

int main() {
    Paragraph *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p = line + 4;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                list = realloc(list, (count + 1) * sizeof(Paragraph));
                list[count].id = strdup(id);
                list[count].text = strdup(text);
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].id, id) == 0) {
                        size_t old_len = strlen(list[i].text);
                        size_t add_len = strlen(text);
                        list[i].text = realloc(list[i].text, old_len + add_len + 1);
                        strcat(list[i].text, text);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *p = line + 8;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].id, id) == 0) {
                        free(list[i].text);
                        list[i].text = strdup(text);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    free(list[i].id);
                    free(list[i].text);
                    for (int j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *id = line + 5;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    print_underscores(list[i].text);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", list[i].id);
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
