// F012.c
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
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "JOIN ", 5) == 0) {
            char name[256];
            char note[1024];
            if (sscanf(line + 5, "%255s %[^\n]", name, note) == 2) {
                Person *tmp = realloc(queue, (count + 1) * sizeof(Person));
                if (tmp) {
                    queue = tmp;
                    queue[count].name = strdup(name);
                    queue[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(line, "SERVE") == 0) {
            if (count > 0) {
                printf("%s %s\n", queue[0].name, queue[0].note);
                free(queue[0].name);
                free(queue[0].note);
                for (int j = 0; j < count - 1; j++) {
                    queue[j] = queue[j + 1];
                }
                count--;
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') {
                name++;
            }
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
        } else if (strncmp(line, "REJOIN ", 7) == 0) {
            char name[256];
            char note[1024];
            if (sscanf(line + 7, "%255s %[^\n]", name, note) == 2) {
                Person *tmp = realloc(queue, (count + 1) * sizeof(Person));
                if (tmp) {
                    queue = tmp;
                    queue[count].name = strdup(name);
                    queue[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') {
                name++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
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
