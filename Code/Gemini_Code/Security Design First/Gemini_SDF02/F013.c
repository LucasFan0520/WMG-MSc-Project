/* F013.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

int main(void) {
    Task *tasks = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;

        if (strcmp(cmd, "TODO") == 0) {
            char *title_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note_str = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Task *temp = realloc(tasks, new_cap * sizeof(Task));
                if (!temp) break;
                tasks = temp;
                capacity = new_cap;
            }
            tasks[count].title = strdup(title_str);
            tasks[count].note = strdup(note_str);
            tasks[count].status = 0;
            if (tasks[count].title && tasks[count].note) {
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title_str) == 0 && tasks[i].status == 0) {
                    tasks[i].status = 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title_str) == 0 && tasks[i].status == 1) {
                    tasks[i].status = 2;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title_str) == 0) {
                    free(tasks[i].title);
                    free(tasks[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        tasks[j] = tasks[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title_str) == 0) {
                    const char *st = tasks[i].status == 0 ? "TODO" : (tasks[i].status == 1 ? "DOING" : "DONE");
                    printf("%s %s\n", st, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("TODO:\n");
            for (size_t i = 0; i < count; i++) {
                if (tasks[i].status == 0) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DOING:\n");
            for (size_t i = 0; i < count; i++) {
                if (tasks[i].status == 1) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DONE:\n");
            for (size_t i = 0; i < count; i++) {
                if (tasks[i].status == 2) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
    }
    free(tasks);
    return 0;
}
