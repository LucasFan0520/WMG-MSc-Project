/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

int main(void) {
    Person *queue = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "JOIN ", 5) == 0 || strncmp(line, "REJOIN ", 7) == 0) {
            char *args = (line[0] == 'J') ? line + 5 : line + 7;
            char *space = strchr(args, ' ');
            char *n = NULL;
            char *nt = NULL;
            if (space) {
                *space = '\0';
                n = strdup(args);
                nt = strdup(space + 1);
                *space = ' ';
            } else {
                n = strdup(args);
                nt = strdup("");
            }
            if (n && nt) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Person *tmp = realloc(queue, new_cap * sizeof(Person));
                    if (tmp) {
                        queue = tmp;
                        capacity = new_cap;
                    } else {
                        free(n);
                        free(nt);
                        continue;
                    }
                }
                queue[count].name = n;
                queue[count].note = nt;
                count++;
            } else {
                free(n);
                free(nt);
            }
        } else if (strcmp(line, "SERVE") == 0) {
            if (count > 0) {
                printf("%s %s\n", queue[0].name, queue[0].note);
                free(queue[0].name);
                free(queue[0].note);
                for (int i = 0; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *n = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, n) == 0) {
                    free(queue[i].name);
                    free(queue[i].note);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *n = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, n) == 0) {
                    printf("%s\n", queue[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "QUEUE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", queue[i].name, queue[i].note);
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
