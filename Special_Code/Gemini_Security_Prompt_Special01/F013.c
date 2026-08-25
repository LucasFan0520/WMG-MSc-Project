// F013.c
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
    char *note;
    char *status;
} Task;

int main(void) {
    Task *tasks = NULL;
    size_t count = 0;
    size_t capacity = 0;
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
        if (strcmp(cmd, "TODO") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            char *note = title_end;
            if (*title_end != '\0') {
                *title_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Task *new_t = realloc(tasks, capacity * sizeof(Task));
                if (new_t) tasks = new_t;
            }
            tasks[count].title = mystrdup(title);
            tasks[count].note = mystrdup(note);
            tasks[count].status = mystrdup("TODO");
            count++;
        } else if (strcmp(cmd, "START") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && strcmp(tasks[i].status, "TODO") == 0) {
                    free(tasks[i].status);
                    tasks[i].status = mystrdup("DOING");
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && (strcmp(tasks[i].status, "DOING") == 0 || strcmp(tasks[i].status, "TODO") == 0)) {
                    free(tasks[i].status);
                    tasks[i].status = mystrdup("DONE");
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    free(tasks[i].title);
                    free(tasks[i].note);
                    free(tasks[i].status);
                    for (size_t j = i; j < count - 1; j++) {
                        tasks[j] = tasks[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    printf("%s %s\n", tasks[i].status, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].status, "TODO") == 0) {
                    printf("TODO %s %s\n", tasks[i].title, tasks[i].note);
                }
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].status, "DOING") == 0) {
                    printf("DOING %s %s\n", tasks[i].title, tasks[i].note);
                }
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].status, "DONE") == 0) {
                    printf("DONE %s %s\n", tasks[i].title, tasks[i].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
        free(tasks[i].status);
    }
    free(tasks);
    return 0;
}
