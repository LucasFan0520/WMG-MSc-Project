// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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

int main() {
    Paragraph *items = NULL;
    size_t count = 0;
    size_t cap = 0;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].id);
            }
            free(line);
            continue;
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
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Paragraph *nb = realloc(items, cap * sizeof(Paragraph));
                if (nb) items = nb;
            }
            items[count].id = strdup(id);
            items[count].text = strdup(text);
            count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    size_t nlen = strlen(items[i].text) + strlen(text) + 1;
                    char *nt = malloc(nlen);
                    if (nt) {
                        strcpy(nt, items[i].text);
                        strcat(nt, text);
                        free(items[i].text);
                        items[i].text = nt;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].text);
                    items[i].text = strdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
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
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    found = 1;
                    char *tptr = items[i].text;
                    while (*tptr) {
                        if (*tptr == ' ') {
                            putchar('_');
                        } else {
                            putchar(*tptr);
                        }
                        tptr++;
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
        free(items[i].id);
        free(items[i].text);
    }
    free(items);
    return 0;
}
