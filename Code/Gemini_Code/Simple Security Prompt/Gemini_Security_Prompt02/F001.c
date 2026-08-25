// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main() {
    Paragraph *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *id_start = line + 4;
            char *space = strchr(id_start, ' ');
            char *text_start = "";
            if (space) {
                *space = '\0';
                text_start = space + 1;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id_start) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Paragraph));
                }
                arr[count].id = strdup(id_start);
                arr[count].text = strdup(text_start);
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *id_start = line + 7;
            char *space = strchr(id_start, ' ');
            char *text_start = "";
            if (space) {
                *space = '\0';
                text_start = space + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id_start) == 0) {
                    size_t new_len = strlen(arr[i].text) + strlen(text_start) + 1;
                    arr[i].text = realloc(arr[i].text, new_len);
                    strcat(arr[i].text, text_start);
                    break;
                }
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *id_start = line + 8;
            char *space = strchr(id_start, ' ');
            char *text_start = "";
            if (space) {
                *space = '\0';
                text_start = space + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id_start) == 0) {
                    free(arr[i].text);
                    arr[i].text = strdup(text_start);
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id_start = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id_start) == 0) {
                    free(arr[i].id);
                    free(arr[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *id_start = line + 5;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id_start) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *t = arr[found].text;
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].id);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].text);
    }
    free(arr);
    return 0;
}
