/* F027.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

static Reminder *reminders = NULL;
static int rcount = 0;
static int rcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_reminder(const char *title) {
    for (int i = 0; i < rcount; i++)
        if (strcmp(reminders[i].title, title) == 0) return i;
    return -1;
}

static int cmp_priority(const void *a, const void *b) {
    const Reminder *ra = (const Reminder *)a;
    const Reminder *rb = (const Reminder *)b;
    return rb->priority - ra->priority;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            int pri = 0;
            char title[256] = {0};
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, "%d%n %255s%n", &pri, &off2, title, &off3) != 2) continue;
            const char *note = line + offset + off2 + off3;
            while (*note == ' ') note++;
            if (rcount >= rcap) {
                rcap = rcap == 0 ? 8 : rcap * 2;
                reminders = realloc(reminders, sizeof(Reminder) * rcap);
            }
            reminders[rcount].priority = pri;
            reminders[rcount].title = dup_str(title);
            reminders[rcount].note = dup_str(note);
            rcount++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256] = {0};
            int delta = 0;
            if (sscanf(line + offset, "%255s %d", title, &delta) != 2) continue;
            int idx = find_reminder(title);
            if (idx >= 0) reminders[idx].priority += delta;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256] = {0};
            if (sscanf(line + offset, "%255s", title) != 1) continue;
            int idx = find_reminder(title);
            if (idx < 0) continue;
            free(reminders[idx].title);
            free(reminders[idx].note);
            for (int i = idx; i < rcount - 1; i++) reminders[i] = reminders[i + 1];
            rcount--;
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = 0;
            if (sscanf(line + offset, "%d", &n) != 1) continue;
            if (n > rcount) n = rcount;
            Reminder *sorted = malloc(sizeof(Reminder) * rcount);
            if (!sorted) continue;
            memcpy(sorted, reminders, sizeof(Reminder) * rcount);
            qsort(sorted, rcount, sizeof(Reminder), cmp_priority);
            for (int i = 0; i < n; i++) {
                printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
            }
            free(sorted);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    return 0;
}