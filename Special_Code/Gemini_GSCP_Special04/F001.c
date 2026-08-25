/* F001.c */
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *id;
    char *text;
} Paragraph;

int main(void) {
    Paragraph *notebook = NULL;
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
        if (*p == ' ') {
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *text = p;
        if (strcmp(cmd, "NEW") == 0) {
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *nt = mystrdup(text);
                if (nt) {
                    free(notebook[found].text);
                    notebook[found].text = nt;
                }
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Paragraph *nb = realloc(notebook, cap * sizeof(Paragraph));
                    if (nb) notebook = nb;
                }
                char *nid = mystrdup(id);
                char *ntext = mystrdup(text);
                if (nid && ntext) {
                    notebook[count].id = nid;
                    notebook[count].text = ntext;
                    count++;
                } else {
                    free(nid);
                    free(ntext);
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                size_t nlen = strlen(notebook[found].text) + strlen(text) + 1;
                char *nt = malloc(nlen);
                if (nt) {
                    strcpy(nt, notebook[found].text);
                    strcat(nt, text);
                    free(notebook[found].text);
                    notebook[found].text = nt;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *nt = mystrdup(text);
                if (nt) {
                    free(notebook[found].text);
                    notebook[found].text = nt;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(notebook[found].id);
                free(notebook[found].text);
                for (size_t i = found; i < count - 1; i++) {
                    notebook[i] = notebook[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(notebook[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *t = notebook[found].text;
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", notebook[i].id);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
