// F012.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

int main(void) {
    Person *queue = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "JOIN ", 5) == 0 || strncmp(line, "REJOIN ", 7) == 0) {
            char *args = strncmp(line, "JOIN ", 5) == 0 ? line + 5 : line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *note = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Person *next = realloc(queue, capacity * sizeof(Person));
                    if (!next) break;
                    queue = next;
                }
                queue[count].name = strdup(name);
                queue[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(line, "SERVE") == 0) {
            if (count > 0) {
                printf("%s %s\n", queue[0].name, queue[0].note);
                free(queue[0].name);
                free(queue[0].note);
                for (size_t j = 0; j < count - 1; j++) {
                    queue[j] = queue[j + 1];
                }
                count--;
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "QUEUE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", queue[i].name, queue[i].note);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
    return 0;
}
