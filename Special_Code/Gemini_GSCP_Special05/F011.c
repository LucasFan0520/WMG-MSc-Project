// F011.c
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
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
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
    int has_disc = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *p = line + 7;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *body = "";
            if (*p == ' ') {
                *p = '\0';
                body = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Draft *nb = realloc(drafts, cap * sizeof(Draft));
                if (!nb) {
                    free(line);
                    break;
                }
                drafts = nb;
            }
            drafts[count].title = safe_dup(title);
            drafts[count].body = safe_dup(body);
            count++;
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *p = line + 7;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *body = "";
            if (*p == ' ') {
                *p = '\0';
                body = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    free(drafts[i].body);
                    drafts[i].body = safe_dup(body);
                    break;
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title = line + 8;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    if (has_disc) {
                        free(disc_title);
                        free(disc_body);
                    }
                    disc_title = safe_dup(drafts[i].title);
                    disc_body = safe_dup(drafts[i].body);
                    has_disc = 1;
                    free(drafts[i].title);
                    free(drafts[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (has_disc) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Draft *nb = realloc(drafts, cap * sizeof(Draft));
                    if (nb) {
                        drafts = nb;
                        drafts[count].title = disc_title;
                        drafts[count].body = disc_body;
                        count++;
                        disc_title = NULL;
                        disc_body = NULL;
                        has_disc = 0;
                    }
                } else {
                    drafts[count].title = disc_title;
                    drafts[count].body = disc_body;
                    count++;
                    disc_title = NULL;
                    disc_body = NULL;
                    has_disc = 0;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    printf("%s\n", drafts[i].body);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
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
    if (has_disc) {
        free(disc_title);
        free(disc_body);
    }
    return 0;
}
