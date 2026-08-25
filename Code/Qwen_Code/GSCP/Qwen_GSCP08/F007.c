// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Activity {
    int time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *head = NULL;

void add_activity(int time, char *label, char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = time;
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (!head) {
        head = a;
    } else {
        Activity *last = head;
        while (last->next) last = last->next;
        last->next = a;
    }
}

void delete_activity(int time, char *label) {
    Activity *prev = NULL;
    Activity *curr = head;
    while (curr) {
        if (curr->time == time && strcmp(curr->label, label) == 0) {
            Activity *next = curr->next;
            if (prev) prev->next = next;
            else head = next;
            free(curr->label);
            free(curr->note);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_activity(char *label) {
    for (Activity *a = head; a; a = a->next) {
        if (strcmp(a->label, label) == 0) {
            printf("%d %s %s\n", a->time, a->label, a->note);
            return;
        }
    }
}

void sort_activities() {
    for (Activity *i = head; i; i = i->next) {
        for (Activity *j = i->next; j; j = j->next) {
            if (i->time > j->time || (i->time == j->time && strcmp(i->label, j->label) > 0)) {
                int t = i->time; i->time = j->time; j->time = t;
                char *s = i->label; i->label = j->label; j->label = s;
                s = i->note; i->note = j->note; j->note = s;
            }
        }
    }
    for (Activity *a = head; a; a = a->next) {
        printf("%d %s %s\n", a->time, a->label, a->note);
    }
}

void clear_activities() {
    while (head) {
        Activity *next = head->next;
        free(head->label);
        free(head->note);
        free(head);
        head = next;
    }
}

int main() {
    char cmd[20], label[100], note[1000];
    int time;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d%99s", &time, label);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d%99s", &time, label);
            delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", label);
            find_activity(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_activities();
        }
    }
    return 0;
}