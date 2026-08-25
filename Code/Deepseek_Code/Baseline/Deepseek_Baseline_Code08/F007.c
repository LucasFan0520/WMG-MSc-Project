// F007.c
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

void add_activity(const char *time, const char *label, const char *note) {
    Activity *new = malloc(sizeof(Activity));
    new->time = strdup(time);
    new->label = strdup(label);
    new->note = strdup(note);
    new->next = NULL;
    if (!head) head = new;
    else {
        Activity *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->time, time) == 0 && strcmp(cur->label, label) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->time);
            free(cur->label);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_label(const char *label) {
    Activity *cur = head;
    Activity *earliest = NULL;
    while (cur) {
        if (strcmp(cur->label, label) == 0) {
            if (!earliest || strcmp(cur->time, earliest->time) < 0) {
                earliest = cur;
            }
        }
        cur = cur->next;
    }
    if (earliest) {
        printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
    }
}

void sort_activities() {
    // Simple bubble sort by time then label
    if (!head || !head->next) return;
    int swapped;
    Activity *ptr1, *ptr2;
    do {
        swapped = 0;
        ptr1 = head;
        while (ptr1->next) {
            ptr2 = ptr1->next;
            if (strcmp(ptr1->time, ptr2->time) > 0 ||
                (strcmp(ptr1->time, ptr2->time) == 0 && strcmp(ptr1->label, ptr2->label) > 0)) {
                // swap data
                char *tmp_time = ptr1->time;
                char *tmp_label = ptr1->label;
                char *tmp_note = ptr1->note;
                ptr1->time = ptr2->time;
                ptr1->label = ptr2->label;
                ptr1->note = ptr2->note;
                ptr2->time = tmp_time;
                ptr2->label = tmp_label;
                ptr2->note = tmp_note;
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
    } while (swapped);
}

void clear_all() {
    Activity *cur = head;
    while (cur) {
        Activity *tmp = cur;
        cur = cur->next;
        free(tmp->time);
        free(tmp->label);
        free(tmp->note);
        free(tmp);
    }
    head = NULL;
}

void free_all() {
    clear_all();
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *time = strtok(NULL, " ");
            if (!time) continue;
            char *label = strtok(NULL, " ");
            if (!label) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time = strtok(NULL, " ");
            if (!time) continue;
            char *label = strtok(NULL, " ");
            if (!label) continue;
            delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = strtok(NULL, " ");
            if (label) find_label(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
            Activity *cur = head;
            while (cur) {
                printf("%s %s %s\n", cur->time, cur->label, cur->note);
                cur = cur->next;
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }
    free_all();
    return 0;
}