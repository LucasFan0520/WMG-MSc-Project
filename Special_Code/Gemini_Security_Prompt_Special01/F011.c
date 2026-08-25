// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *drafts = NULL;
    size_t count = 0;
    size_t capacity = 0;
    
    Draft last_discarded = {NULL, NULL};
    
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            char *body = title_end;
            if (*title_end != '\0') {
                *title_end = '\0';
                body++;
            }
            while (*body == ' ') body++;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *new_d = realloc(drafts, capacity * sizeof(Draft));
                    if (new_d) drafts = new_d;
                }
                drafts[count].title = mystrdup(title);
                drafts[count].body = mystrdup(body);
                count++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            char *body = title_end;
            if (*title_end != '\0') {
                *title_end = '\0';
                body++;
            }
            while (*body == ' ') body++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    free(drafts[i].body);
                    drafts[i].body = mystrdup(body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    if (last_discarded.title) free(last_discarded.title);
                    if (last_discarded.body) free(last_discarded.body);
                    last_discarded.title = drafts[i].title;
                    last_discarded.body = drafts[i].body;
                    for (size_t j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (last_discarded.title) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *new_d = realloc(drafts, capacity * sizeof(Draft));
                    if (new_d) drafts = new_d;
                }
                drafts[count].title = last_discarded.title;
                drafts[count].body = last_discarded.body;
                count++;
                last_discarded.title = NULL;
                last_discarded.body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
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
    if (last_discarded.title) free(last_discarded.title);
    if (last_discarded.body) free(last_discarded.body);
    return 0;
}
