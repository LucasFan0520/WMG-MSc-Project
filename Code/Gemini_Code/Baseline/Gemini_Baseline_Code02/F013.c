// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

int main() {
    Task *tasks = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
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
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "TODO") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = args;
                char *note = p2 + 1;
                tasks = realloc(tasks, sizeof(Task) * (count + 1));
                tasks[count].title = strdup(title);
                tasks[count].note = strdup(note);
                tasks[count].status = 0;
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && tasks[i].status == 0) {
                    tasks[i].status = 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && tasks[i].status == 1) {
                    tasks[i].status = 2;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    free(tasks[i].title);
                    free(tasks[i].note);
                    for (int j = i; j < count - 1; j++) {
                        tasks[j] = tasks[j + 1];
                    }
                    count--;
                    if (count == 0) { free(tasks); tasks = NULL; }
                    else { tasks = realloc(tasks, sizeof(Task) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    const char *st_str = (tasks[i].status == 0) ? "TODO" : ((tasks[i].status == 1) ? "DOING" : "DONE");
                    printf("%s %s\n", st_str, tasks[i].note);
                    break;
                }
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
