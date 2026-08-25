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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *note = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Person *tmp = realloc(queue, capacity * sizeof(Person));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        queue = tmp;
                    }
                    queue[count].name = strdup(arg1);
                    queue[count].note = strdup(note);
                    if (queue[count].name && queue[count].note) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "CANCEL") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(queue[i].name, arg1) == 0) {
                        free(queue[i].name);
                        free(queue[i].note);
                        for (int j = i; j < count - 1; j++) {
                            queue[j] = queue[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, arg1) == 0) {
                        printf("%s\n", queue[i].note);
                        break;
                    }
                }
            }
        } else {
            if (strcmp(cmd, "SERVE") == 0) {
                if (count > 0) {
                    printf("%s\n", queue[0].name);
                    free(queue[0].name);
                    free(queue[0].note);
                    for (int i = 0; i < count - 1; i++) {
                        queue[i] = queue[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "QUEUE") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", queue[i].name);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
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