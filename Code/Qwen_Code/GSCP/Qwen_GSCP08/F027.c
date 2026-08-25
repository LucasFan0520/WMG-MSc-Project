// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    char *title;
    char *note;
    int priority;
    struct Reminder *next;
} Reminder;

Reminder *head = NULL;

Reminder *find_reminder(char *title) {
    for (Reminder *r = head; r; r = r->next) {
        if (strcmp(r->title, title) == 0) return r;
    }
    return NULL;
}

void add_reminder(int priority, char *title, char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    r->title = strdup(title);
    r->note = strdup(note);
    r->priority = priority;
    r->next = head;
    head = r;
}

void bump_reminder(char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (r) r->priority += delta;
}

void remove_reminder(char *title) {
    Reminder *prev = NULL;
    Reminder *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void top_reminders(int n) {
    Reminder **arr = malloc(sizeof(Reminder*) * 10000);
    int count = 0;
    for (Reminder *r = head; r; r = r->next) {
        arr[count++] = r;
    }
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i]->priority < arr[j]->priority) {
                Reminder *temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders() {
    for (Reminder *r = head; r; r = r->next) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
    }
}

int main() {
    char cmd[20], a[100], note[1000];
    int p, d, n;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d%99s", &p, a);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            add_reminder(p, a, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            scanf("%99s%d", a, &d);
            bump_reminder(a, d);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%99s", a);
            remove_reminder(a);
        } else if (strcmp(cmd, "TOP") == 0) {
            scanf("%d", &n);
            top_reminders(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    return 0;
}