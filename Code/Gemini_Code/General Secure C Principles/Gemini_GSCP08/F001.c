// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main(void) {
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
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s\n", arr[i].id);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *id = p1 + 1;
        if (strcmp(cmd, "NEW") == 0) {
            char *p2 = strchr(id, ' ');
            if (p2) {
                *p2 = '\0';
                char *text = p2 + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found == -1) {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Paragraph *narr = realloc(arr, ncap * sizeof(Paragraph));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].id = strdup(id);
                        arr[count].text = strdup(text);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p2 = strchr(id, ' ');
            if (p2) {
                *p2 = '\0';
                char *text = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        size_t nlen = strlen(arr[i].text) + strlen(text) + 1;
                        char *ntext = realloc(arr[i].text, nlen);
                        if (ntext) {
                            arr[i].text = ntext;
                            strcat(arr[i].text, text);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *p2 = strchr(id, ' ');
            if (p2) {
                *p2 = '\0';
                char *text = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        char *ntext = strdup(text);
                        if (ntext) {
                            free(arr[i].text);
                            arr[i].text = ntext;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    found = 1;
                    for (size_t j = 0; arr[i].text[j] != '\0'; j++) {
                        if (arr[i].text[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(arr[i].text[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
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
