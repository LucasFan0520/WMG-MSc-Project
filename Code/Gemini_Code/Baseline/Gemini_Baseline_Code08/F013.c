// F013.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

Task *tasks = NULL;
int count = 0;
int capacity = 0;

int find_task(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(tasks[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

void add_todo(const char *title, const char *note) {
    int idx = find_task(title);
    if (idx != -1) {
        free(tasks[idx].note);
        tasks[idx].note = strdup(note);
        tasks[idx].status = 0;
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        tasks = realloc(tasks, capacity * sizeof(Task));
    }
    tasks[count].title = strdup(title);
    tasks[count].note = strdup(note);
    tasks[count].status = 0;
    count++;
}

void start_task(const char *title) {
    int idx = find_task(title);
    if (idx != -1) {
        tasks[idx].status = 1;
    }
}

void done_task(const char *title) {
    int idx = find_task(title);
    if (idx != -1) {
        tasks[idx].status = 2;
    }
}

void remove_task(const char *title) {
    int idx = find_task(title);
    if (idx != -1) {
        free(tasks[idx].title);
        free(tasks[idx].note);
        for (int i = idx; i < count - 1; i++) {
            tasks[i] = tasks[i + 1];
        }
        count--;
    }
}

void find_task_p(const char *title) {
    int idx = find_task(title);
    if (idx != -1) {
        const char *status_str = (tasks[idx].status == 0) ? "TODO" : ((tasks[idx].status == 1) ? "DOING" : "DONE");
        printf("%s %s\n", status_str, tasks[idx].note);
    }
}

void report_tasks(void) {
    for (int s = 0; s < 3; s++) {
        for (int i = 0; i < count; i++) {
            if (tasks[i].status == s) {
                const char *status_str = (s == 0) ? "TODO" : ((s == 1) ? "DOING" : "DONE");
                printf("%s %s %s\n", status_str, tasks[i].title, tasks[i].note);
            }
        }
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (title && note) add_todo(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            char *title = strtok(NULL, " ");
            if (title) start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_task_p(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tasks();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
    }
    free(tasks);
    return 0;
}
