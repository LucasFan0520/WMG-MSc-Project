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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main() {
    Paragraph *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '	' || *p == '\n' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ' || *p == '	') p++;
        char *id = p;
        while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        while (*p == ' ' || *p == '	') p++;
        char *text = p;
        size_t tl = strlen(text);
        while (tl > 0 && (text[tl-1] == '\n' || text[tl-1] == '\n')) {
            text[tl-1] = '\0';
            tl--;
        }
        if (strcmp(cmd, "NEW") == 0) {
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Paragraph *narr = realloc(arr, cap * sizeof(Paragraph));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].id = mystrdup(id);
                    arr[count].text = mystrdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    size_t old_len = strlen(arr[i].text);
                    size_t new_len = strlen(text);
                    char *ntext = realloc(arr[i].text, old_len + new_len + 1);
                    if (ntext) {
                        arr[i].text = ntext;
                        strcpy(arr[i].text + old_len, text);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].text);
                    arr[i].text = mystrdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].text);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    found = 1;
                    char *s = arr[i].text;
                    while (*s) {
                        if (*s == ' ') putchar('_');
                        else putchar(*s);
                        s++;
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
                printf("%s\n", arr[i].id);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].text);
    }
    free(arr);
    return 0;
}
