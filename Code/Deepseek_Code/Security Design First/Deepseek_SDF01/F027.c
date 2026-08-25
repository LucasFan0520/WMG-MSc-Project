// filename: F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    int priority;
    char *title;
    char *note;
    struct Reminder *next;
} Reminder;

static Reminder *reminders = NULL;
static Reminder *tail = NULL;

static void cleanup(void) {
    while (reminders) {
        Reminder *tmp = reminders;
        reminders = reminders->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

static Reminder *find_reminder(const char *title) {
    for (Reminder *r = reminders; r; r = r->next)
        if (strcmp(r->title, title) == 0)
            return r;
    return NULL;
}

static void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    if (!r) return;
    r->title = strdup(title);
    r->note = strdup(note);
    if (!r->title || !r->note) {
        free(r->title);
        free(r->note);
        free(r);
        return;
    }
    r->priority = priority;
    r->next = NULL;
    if (tail) tail->next = r;
    else reminders = r;
    tail = r;
}

static void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

static void remove_reminder(const char *title) {
    Reminder *prev = NULL;
    for (Reminder *r = reminders; r; r = r->next) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else reminders = r->next;
            if (!r->next) tail = prev;
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
    }
}

static void top_n(int n) {
    // copy list to array, sort by priority descending, print first n
    int count = 0;
    for (Reminder *r = reminders; r; r = r->next) ++count;
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    if (!arr) return;
    int i = 0;
    for (Reminder *r = reminders; r; r = r->next) arr[i++] = r;
    for (int j = 0; j < count - 1; ++j) {
        for (int k = 0; k < count - j - 1; ++k) {
            if (arr[k]->priority < arr[k+1]->priority) {
                Reminder *tmp = arr[k];
                arr[k] = arr[k+1];
                arr[k+1] = tmp;
            }
        }
    }
    int limit = (n < count) ? n : count;
    for (int j = 0; j < limit; ++j)
        printf("%d %s %s\n", arr[j]->priority, arr[j]->title, arr[j]->note);
    free(arr);
}

static void list_reminders(void) {
    for (Reminder *r = reminders; r; r = r->next)
        printf("%d %s %s\n", r->priority, r->title, r->note);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        int pri = atoi(arg1);
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_reminder(pri, arg2, arg3);
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (*arg1 && *arg2) bump_priority(arg1, atoi(arg2));
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (*arg1) remove_reminder(arg1);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (*arg1) top_n(atoi(arg1));
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free(line);
    return 0;
}