/* F011.c */
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
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *drafts = NULL;
    size_t count = 0;
    size_t cap = 0;
    char *disc_title = NULL;
    char *disc_body = NULL;
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
        if (strcmp(cmd, "CREATE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Draft *nd = realloc(drafts, cap * sizeof(Draft));
                    if (nd) drafts = nd;
                }
                char *nt = mystrdup(title);
                char *nb = mystrdup(body);
                if (nt && nb) {
                    drafts[count].title = nt;
                    drafts[count].body = nb;
                    count++;
                } else {
                    free(nt);
                    free(nb);
                }
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *nb = mystrdup(body);
                if (nb) {
                    free(drafts[found].body);
                    drafts[found].body = nb;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(disc_title);
                free(disc_body);
                disc_title = drafts[found].title;
                disc_body = drafts[found].body;
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    drafts[i] = drafts[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (disc_title) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Draft *nd = realloc(drafts, cap * sizeof(Draft));
                    if (nd) drafts = nd;
                }
                drafts[count].title = disc_title;
                drafts[count].body = disc_body;
                count++;
                disc_title = NULL;
                disc_body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s\n", drafts[found].body);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    free(disc_title);
    free(disc_body);
    return 0;
}
