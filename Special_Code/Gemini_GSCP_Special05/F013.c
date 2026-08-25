// F013.c
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
    char *note;
    char *status;
} Task;

int main(void) {
    Task *tasks = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "TODO ", 5) == 0) {
            char *p = line + 5;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Task *nb = realloc(tasks, cap * sizeof(Task));
                if (!nb) {
                    free(line);
                    break;
                }
                tasks = nb;
            }
            tasks[count].title = safe_dup(title);
            tasks[count].note = safe_dup(note);
            tasks[count].status = safe_dup("TODO");
            count++;
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title = line + 6;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && strcmp(tasks[i].status, "TODO") == 0) {
                    free(tasks[i].status);
                    tasks[i].status = safe_dup("DOING");
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && strcmp(tasks[i].status, "DOING") == 0) {
                    free(tasks[i].status);
                    tasks[i].status = safe_dup("DONE");
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    printf("%s %s\n", tasks[i].status, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", tasks[i].status, tasks[i].title, tasks[i].note);
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
