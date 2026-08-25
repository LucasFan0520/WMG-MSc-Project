// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 100
#define MAX_NOTE 1000

typedef struct {
    char title[MAX_TITLE];
    char note[MAX_NOTE];
    int priority;
    int order;
} Reminder;

Reminder reminders[10000];
int rem_count = 0;
int global_order = 0;

int find_rem(const char *title) {
    for (int i = 0; i < rem_count; i++) if (strcmp(reminders[i].title, title) == 0) return i;
    return -1;
}

int cmp_priority(const void *a, const void *b) {
    Reminder *ra = (Reminder *)a;
    Reminder *rb = (Reminder *)b;
    if (rb->priority != ra->priority) return rb->priority - ra->priority;
    return ra->order - rb->order;
}

int main() {
    char cmd[20], title[MAX_TITLE], note[MAX_NOTE];
    int delta, n;
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d %s", &delta, title);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            strcpy(reminders[rem_count].title, title);
            strcpy(reminders[rem_count].note, note);
            reminders[rem_count].priority = delta;
            reminders[rem_count].order = global_order++;
            rem_count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            scanf("%s %d", title, &delta);
            int idx = find_rem(title);
            if (idx != -1) reminders[idx].priority += delta;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", title);
            int idx = find_rem(title);
            if (idx != -1) {
                for (int i = idx; i < rem_count - 1; i++) reminders[i] = reminders[i + 1];
                rem_count--;
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            scanf("%d", &n);
            Reminder temp[10000];
            memcpy(temp, reminders, sizeof(Reminder) * rem_count);
            qsort(temp, rem_count, sizeof(Reminder), cmp_priority);
            int limit = n < rem_count ? n : rem_count;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rem_count; i++) {
                printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
    }
    return 0;
}