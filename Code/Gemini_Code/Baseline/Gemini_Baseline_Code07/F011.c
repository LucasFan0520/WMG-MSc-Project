// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main() {
    Draft *drafts = NULL;
    int count = 0;
    int capacity = 0;
    char *last_discarded_title = NULL;
    char *last_discarded_body = NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "RESTORE") == 0) {
            if (last_discarded_title) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    drafts = realloc(drafts, capacity * sizeof(Draft));
                }
                drafts[count].title = strdup(last_discarded_title);
                drafts[count].body = strdup(last_discarded_body);
                count++;
                free(last_discarded_title);
                free(last_discarded_body);
                last_discarded_title = NULL;
                last_discarded_body = NULL;
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "CREATE") == 0) {
            char title[256];
            int n2 = 0;
            if (sscanf(p, "%255s%n", title, &n2) == 1) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    drafts = realloc(drafts, capacity * sizeof(Draft));
                }
                drafts[count].title = strdup(title);
                drafts[count].body = strdup(p);
                count++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[256];
            int n2 = 0;
            if (sscanf(p, "%255s%n", title, &n2) == 1) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        free(drafts[i].body);
                        drafts[i].body = strdup(p);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256];
            if (sscanf(p, "%255s", title) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(last_discarded_title);
                    free(last_discarded_body);
                    last_discarded_title = strdup(drafts[found].title);
                    last_discarded_body = strdup(drafts[found].body);
                    free(drafts[found].title);
                    free(drafts[found].body);
                    for (int i = found; i < count - 1; i++) {
                        drafts[i] = drafts[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(p, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        printf("%s\n", drafts[i].body);
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    free(last_discarded_title);
    free(last_discarded_body);
    return 0;
}
