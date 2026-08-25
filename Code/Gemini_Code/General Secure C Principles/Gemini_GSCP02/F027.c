// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    Reminder *rA = (Reminder *)a;
    Reminder *rB = (Reminder *)b;
    return (rB->priority > rA->priority) - (rB->priority < rA->priority);
}

int main(void) {
    Reminder *reminders = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            int prio;
            char title[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %255s", &prio, title) == 2) {
                char *note = strchr(p, ' ');
                if (note) {
                    note = strchr(note + 1, ' ');
                    if (note) {
                        while (*note == ' ') note++;
                    }
                }
                if (!note) note = "";
                Reminder *tmp = realloc(reminders, (count + 1) * sizeof(Reminder));
                if (tmp) {
                    reminders = tmp;
                    reminders[count].priority = prio;
                    reminders[count].title = strdup(title);
                    reminders[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char title[256];
            int delta;
            if (sscanf(line + 5, "%255s %d", title, &delta) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(reminders[i].title, title) == 0) {
                        reminders[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    free(reminders[i].title);
                    free(reminders[i].note);
                    for (int j = i; j < count - 1; j++) {
                        reminders[j] = reminders[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            int n;
            if (sscanf(line + 4, "%d", &n) == 1) {
                Reminder *copy = malloc(count * sizeof(Reminder));
                if (copy) {
                    memcpy(copy, reminders, count * sizeof(Reminder));
                    qsort(copy, count, sizeof(Reminder), compare_reminders);
                    int limit = (n < count) ? n : count;
                    for (int i = 0; i < limit; i++) {
                        printf("(%d) %s: %s\n", copy[i].priority, copy[i].title, copy[i].note);
                    }
                    free(copy);
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("(%d) %s: %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    return 0;
}
