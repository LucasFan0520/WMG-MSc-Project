// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

int main(void) {
    Task *tasks = NULL;
    int count = 0;
    int capacity = 0;
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
            char *title_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note_start = p + 1;
                while (*note_start == ' ') note_start++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Task *new_t = realloc(tasks, capacity * sizeof(Task));
                    if (new_t) tasks = new_t;
                }
                if (count < capacity) {
                    tasks[count].title = strdup(title_start);
                    tasks[count].note = strdup(note_start);
                    tasks[count].status = 0;
                    count++;
                }
            }
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title_start = line + 6;
            while (*title_start == ' ') title_start++;
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 0 && strcmp(tasks[i].title, title_start) == 0) {
                    tasks[i].status = 1;
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title_start = line + 5;
            while (*title_start == ' ') title_start++;
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 1 && strcmp(tasks[i].title, title_start) == 0) {
                    tasks[i].status = 2;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title_start = line + 7;
            while (*title_start == ' ') title_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title_start) == 0) {
                    free(tasks[i].title);
                    free(tasks[i].note);
                    for (int j = i; j < count - 1; j++) {
                        tasks[j] = tasks[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title_start = line + 5;
            while (*title_start == ' ') title_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title_start) == 0) {
                    const char *st_str = tasks[i].status == 0 ? "TODO" : (tasks[i].status == 1 ? "DOING" : "DONE");
                    printf("%s %s\n", st_str, tasks[i].note);
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
