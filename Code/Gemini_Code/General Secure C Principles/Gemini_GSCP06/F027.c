// F027.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    size_t id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *aa = (const Reminder *)a;
    const Reminder *bb = (const Reminder *)b;
    if (aa->priority != bb->priority) {
        return bb->priority - aa->priority;
    }
    return (aa->id < bb->id) ? -1 : 1;
}

int main(void) {
    Reminder *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    size_t global_id = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                int prio = atoi(args);
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *title = rest;
                    char *note = sp2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Reminder *next = realloc(list, capacity * sizeof(Reminder));
                        if (!next) break;
                        list = next;
                    }
                    list[count].priority = prio;
                    list[count].title = strdup(title);
                    list[count].note = strdup(note);
                    list[count].id = global_id++;
                    count++;
                }
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *title = args;
                int delta = atoi(space + 1);
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(list[i].title, title) == 0) {
                        list[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    free(list[i].title);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            int n = atoi(line + 4);
            if (n > 0 && count > 0) {
                Reminder *temp = malloc(count * sizeof(Reminder));
                if (temp) {
                    memcpy(temp, list, count * sizeof(Reminder));
                    qsort(temp, count, sizeof(Reminder), compare_reminders);
                    size_t limit = (size_t)n < count ? (size_t)n : count;
                    for (size_t i = 0; i < limit; i++) {
                        printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
                    }
                    free(temp);
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%d %s %s\n", list[i].priority, list[i].title, list[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].note);
    }
    free(list);
    free(line);
    return 0;
}
