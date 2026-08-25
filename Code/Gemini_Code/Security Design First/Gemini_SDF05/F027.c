// F027.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    int id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *aa = *(const Reminder **)a;
    const Reminder *bb = *(const Reminder **)b;
    if (aa->priority > bb->priority) return -1;
    if (aa->priority < bb->priority) return 1;
    if (aa->id < bb->id) return -1;
    if (aa->id > bb->id) return 1;
    return 0;
}

int main(void) {
    Reminder *reminders = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int total_inserted = 0;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *p_str = p;
            char *space1 = strchr(p_str, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *title = space1 + 1;
            while (*title == ' ') title++;
            char *space2 = strchr(title, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            int prio = atoi(p_str);
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Reminder *tmp = realloc(reminders, new_cap * sizeof(Reminder));
                if (!tmp) return 1;
                reminders = tmp;
                capacity = new_cap;
            }
            reminders[count].priority = prio;
            reminders[count].title = strdup(title);
            reminders[count].note = strdup(note);
            reminders[count].id = total_inserted++;
            count++;
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *d_str = space + 1;
            while (*d_str == ' ') d_str++;
            int delta = atoi(d_str);
            for (int i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    reminders[i].priority += delta;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(reminders[found].title);
                free(reminders[found].note);
                for (int i = found; i < count - 1; i++) {
                    reminders[i] = reminders[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            int n = atoi(p);
            if (n > count) n = count;
            if (n > 0) {
                Reminder **arr = malloc(count * sizeof(Reminder *));
                if (!arr) return 1;
                for (int i = 0; i < count; i++) {
                    arr[i] = &reminders[i];
                }
                qsort(arr, count, sizeof(Reminder *), compare_reminders);
                for (int i = 0; i < n; i++) {
                    printf("(%d) %s: %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
                }
                free(arr);
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
