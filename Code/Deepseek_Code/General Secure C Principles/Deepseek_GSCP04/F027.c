/* F027.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    char *title;
    char *note;
    int priority;
} Reminder;

static Reminder *reminders = NULL;
static size_t rcount = 0, rcap = 0;

static Reminder *find_reminder(const char *title) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(reminders[i].title, title) == 0)
            return &reminders[i];
    }
    return NULL;
}

static void add_reminder(int priority, const char *title, const char *note) {
    if (rcount == rcap) {
        rcap = rcap ? rcap * 2 : 4;
        reminders = realloc(reminders, rcap * sizeof(Reminder));
        if (!reminders) { perror("realloc"); exit(1); }
    }
    reminders[rcount].priority = priority;
    reminders[rcount].title = strdup(title);
    reminders[rcount].note = strdup(note);
    rcount++;
}

static void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (r) r->priority += delta;
}

static void remove_reminder(const char *title) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(reminders[i].title, title) == 0) {
            free(reminders[i].title);
            free(reminders[i].note);
            for (size_t j = i; j < rcount - 1; j++)
                reminders[j] = reminders[j + 1];
            rcount--;
            return;
        }
    }
}

static int cmp_priority(const void *a, const void *b) {
    const Reminder *ra = (const Reminder *)a;
    const Reminder *rb = (const Reminder *)b;
    return rb->priority - ra->priority; // descending
}

static void top_n(int n) {
    if (n > (int)rcount) n = (int)rcount;
    qsort(reminders, rcount, sizeof(Reminder), cmp_priority);
    for (int i = 0; i < n; i++) {
        printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
    }
}

static void list_reminders(void) {
    for (size_t i = 0; i < rcount; i++) {
        printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *pstr = strtok(NULL, " ");
            if (!pstr) continue;
            int pri = atoi(pstr);
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            if (find_reminder(title)) {
                Reminder *r = find_reminder(title);
                r->priority = pri;
                free(r->note);
                r->note = strdup(note);
            } else {
                add_reminder(pri, title, note);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *dstr = strtok(NULL, " ");
            if (!dstr) continue;
            int delta = atoi(dstr);
            bump_priority(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            char *nstr = strtok(NULL, " ");
            if (!nstr) continue;
            int n = atoi(nstr);
            top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    for (size_t i = 0; i < rcount; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    return 0;
}