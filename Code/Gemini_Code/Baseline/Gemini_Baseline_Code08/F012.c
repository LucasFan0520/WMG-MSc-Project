// F012.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

Person *queue = NULL;
int count = 0;
int capacity = 0;

void join_q(const char *name, const char *note) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        queue = realloc(queue, capacity * sizeof(Person));
    }
    queue[count].name = strdup(name);
    queue[count].note = strdup(note);
    count++;
}

void serve_q(void) {
    if (count > 0) {
        printf("%s %s\n", queue[0].name, queue[0].note);
        free(queue[0].name);
        free(queue[0].note);
        for (int i = 0; i < count - 1; i++) {
            queue[i] = queue[i + 1];
        }
        count--;
    }
}

void cancel_q(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(queue[i].name, name) == 0) {
            free(queue[i].name);
            free(queue[i].note);
            for (int j = i; j < count - 1; j++) {
                queue[j] = queue[j + 1];
            }
            count--;
            break;
        }
    }
}

void find_q(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(queue[i].name, name) == 0) {
            printf("%s\n", queue[i].note);
            return;
        }
    }
}

void print_q(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", queue[i].name, queue[i].note);
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
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && note) join_q(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve_q();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(NULL, " ");
            if (name) cancel_q(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_q(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_q();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
    return 0;
}
