// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *drafts = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *disc_title = NULL;
    char *disc_body = NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CREATE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *body = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && *title != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *new_drafts = realloc(drafts, capacity * sizeof(Draft));
                    if (new_drafts) drafts = new_drafts;
                }
                if (count < capacity) {
                    drafts[count].title = strdup(title);
                    drafts[count].body = strdup(body);
                    count++;
                }
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *body = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    free(drafts[i].body);
                    drafts[i].body = strdup(body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    free(disc_title);
                    free(disc_body);
                    disc_title = strdup(drafts[i].title);
                    disc_body = strdup(drafts[i].body);
                    free(drafts[i].title);
                    free(drafts[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (disc_title) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *new_drafts = realloc(drafts, capacity * sizeof(Draft));
                    if (new_drafts) drafts = new_drafts;
                }
                if (count < capacity) {
                    drafts[count].title = strdup(disc_title);
                    drafts[count].body = strdup(disc_body);
                    count++;
                }
                free(disc_title);
                free(disc_body);
                disc_title = NULL;
                disc_body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    printf("%s\n", drafts[i].body);
                    break;
                }
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
