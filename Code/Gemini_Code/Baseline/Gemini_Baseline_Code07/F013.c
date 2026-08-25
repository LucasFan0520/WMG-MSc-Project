// F013.c
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
    char *note;
    char *status;
} Task;

int main() {
    Task *tasks = NULL;
    int count = 0;
    int capacity = 0;
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
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", tasks[i].status, tasks[i].title, tasks[i].note);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "TODO") == 0) {
            char title[256];
            int n2 = 0;
            if (sscanf(p, "%255s%n", title, &n2) == 1) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    tasks = realloc(tasks, capacity * sizeof(Task));
                }
                tasks[count].title = strdup(title);
                tasks[count].note = strdup(p);
                tasks[count].status = strdup("TODO");
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            char title[256];
            if (sscanf(p, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0 && strcmp(tasks[i].status, "TODO") == 0) {
                        free(tasks[i].status);
                        tasks[i].status = strdup("DOING");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[256];
            if (sscanf(p, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0 && strcmp(tasks[i].status, "DOING") == 0) {
                        free(tasks[i].status);
                        tasks[i].status = strdup("DONE");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(p, "%255s", title) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(tasks[found].title);
                    free(tasks[found].note);
                    free(tasks[found].status);
                    for (int i = found; i < count - 1; i++) {
                        tasks[i] = tasks[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(p, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0) {
                        printf("%s %s\n", tasks[i].status, tasks[i].note);
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
        free(tasks[i].status);
    }
    free(tasks);
    return 0;
}
