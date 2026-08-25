// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    int priority;
    char *title;
    char *note;
    struct Reminder *next;
} Reminder;

Reminder *head = NULL;
Reminder *tail = NULL;

Reminder *find_by_title(const char *title) {
    Reminder *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_by_title(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    r->priority = priority;
    r->title = malloc(strlen(title) + 1);
    strcpy(r->title, title);
    r->note = malloc(strlen(note) + 1);
    strcpy(r->note, note);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void bump_priority(const char *title, int delta) {
    Reminder *r = find_by_title(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL;
    Reminder *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (tail == r) tail = prev;
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

int compare_priority(const void *a, const void *b) {
    Reminder *ra = *(Reminder**)a;
    Reminder *rb = *(Reminder**)b;
    return rb->priority - ra->priority; // descending
}

void top_n(int n) {
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    if (n > count) n = count;
    Reminder **arr = malloc(count * sizeof(Reminder*));
    int i = 0;
    r = head;
    while (r) { arr[i++] = r; r = r->next; }
    qsort(arr, count, sizeof(Reminder*), compare_priority);
    for (i = 0; i < n; i++) {
        printf("%d %s ", arr[i]->priority, arr[i]->title);
        for (char *c = arr[i]->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
    free(arr);
}

void list_reminders(void) {
    Reminder *r = head;
    while (r) {
        printf("%d %s ", r->priority, r->title);
        for (char *c = r->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        r = r->next;
    }
}

void free_all(void) {
    Reminder *r = head;
    while (r) {
        Reminder *next = r->next;
        free(r->title);
        free(r->note);
        free(r);
        r = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            int prio;
            if (sscanf(p, "%d", &prio) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_reminder(prio, title, p);
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            int delta;
            if (sscanf(p, "%d", &delta) != 1) continue;
            bump_priority(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_reminder(p);
        } else if (strcmp(cmd, "TOP") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            int n;
            if (sscanf(p, "%d", &n) != 1) continue;
            top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free_all();
    return 0;
}