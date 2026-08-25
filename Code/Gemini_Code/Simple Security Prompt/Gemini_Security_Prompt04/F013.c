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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

int main(void) {
    Task *tasks = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "TODO ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                while (*note == ' ') note++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    tasks = realloc(tasks, cap * sizeof(Task));
                }
                tasks[count].title = strdup(title);
                tasks[count].note = strdup(note);
                tasks[count].status = 0;
                count++;
            }
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title = line + 6;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && tasks[i].status == 0) {
                    tasks[i].status = 1;
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && tasks[i].status == 1) {
                    tasks[i].status = 2;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
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
                for (int i = found; i < count - 1; i++) {
                    tasks[i] = tasks[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    const char *st = tasks[i].status == 0 ? "todo" : (tasks[i].status == 1 ? "doing" : "done");
                    printf("%s %s\n", st, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            printf("TODO:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 0) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DOING:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 1) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DONE:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 2) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
    }
    free(tasks);
    return 0;
}
