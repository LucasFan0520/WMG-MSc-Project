/* F007.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Activity {
    char *time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *head = NULL;
Activity *tail = NULL;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (tail) tail->next = a;
    else head = a;
    tail = a;
}

void delete_activity(const char *time, const char *label) {
    Activity *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            if (a == tail) tail = prev;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void find_label(const char *label) {
    Activity *a = head;
    Activity *earliest = NULL;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0) earliest = a;
        }
        a = a->next;
    }
    if (earliest) {
        printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
    }
}

void sort_activities() {
    if (!head) return;
    Activity *sorted = NULL;
    Activity *a = head;
    while (a) {
        Activity *next = a->next;
        if (!sorted || strcmp(a->time, sorted->time) < 0 ||
            (strcmp(a->time, sorted->time) == 0 && strcmp(a->label, sorted->label) < 0)) {
            a->next = sorted;
            sorted = a;
        } else {
            Activity *cur = sorted;
            while (cur->next && (strcmp(cur->next->time, a->time) < 0 ||
                (strcmp(cur->next->time, a->time) == 0 && strcmp(cur->next->label, a->label) < 0))) {
                cur = cur->next;
            }
            a->next = cur->next;
            cur->next = a;
        }
        a = next;
    }
    head = sorted;
    tail = NULL;
    for (a = head; a; a = a->next) tail = a;
}

void print_sorted() {
    sort_activities();
    Activity *a = head;
    while (a) {
        printf("%s %s %s\n", a->time, a->label, a->note);
        a = a->next;
    }
}

void clear_all() {
    while (head) {
        Activity *tmp = head;
        head = head->next;
        free(tmp->time);
        free(tmp->label);
        free(tmp->note);
        free(tmp);
    }
    tail = NULL;
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char time[256]; char label[256]; char note[4096];
        int n = sscanf(line, "%31s %255s %255s %[^\n]", cmd, time, label, note);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 3) {
            delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_label(time);
        } else if (strcmp(cmd, "SORT") == 0 && n == 1) {
            print_sorted();
        } else if (strcmp(cmd, "CLEAR") == 0 && n == 1) {
            clear_all();
        }
    }
    clear_all();
    return 0;
}