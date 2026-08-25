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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *drafts = NULL;
    int count = 0;
    int cap = 0;
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
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *body = p + 1;
                while (*body == ' ') body++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    drafts = realloc(drafts, cap * sizeof(Draft));
                }
                drafts[count].title = strdup(title);
                drafts[count].body = strdup(body);
                count++;
            }
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *body = p + 1;
                while (*body == ' ') body++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        free(drafts[i].body);
                        drafts[i].body = strdup(body);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title = line + 8;
            while (*title == ' ') title++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                if (has_disc) {
                    free(disc_title);
                    free(disc_body);
                }
                disc_title = strdup(drafts[found].title);
                disc_body = strdup(drafts[found].body);
                has_disc = 1;
                free(drafts[found].title);
                free(drafts[found].body);
                for (int i = found; i < count - 1; i++) {
                    drafts[i] = drafts[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (has_disc) {
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    drafts = realloc(drafts, cap * sizeof(Draft));
                }
                drafts[count].title = strdup(disc_title);
                drafts[count].body = strdup(disc_body);
                count++;
                free(disc_title);
                free(disc_body);
                disc_title = NULL;
                disc_body = NULL;
                has_disc = 0;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    printf("%s\n", drafts[i].body);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
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
