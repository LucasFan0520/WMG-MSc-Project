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

void add_activity(int time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    a->time = time;
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    
    if (!head) {
        head = a;
    } else {
        Activity *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = a;
    }
}

void delete_activity(int time, const char *label) {
    Activity *curr = head;
    Activity *prev = NULL;
    while (curr) {
        if (curr->time == time && strcmp(curr->label, label) == 0) {
            Activity *next = curr->next;
            free(curr->label);
            free(curr->note);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_activity(const char *label) {
    Activity *earliest = NULL;
    for (Activity *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->label, label) == 0) {
            if (!earliest || curr->time < earliest->time) {
                earliest = curr;
            }
        }
    }
    if (earliest) {
        printf("%d %s %s\n", earliest->time, earliest->label, earliest->note);
    }
}

void sort_activities() {
    if (!head || !head->next) return;
    
    int swapped;
    do {
        swapped = 0;
        Activity *curr = head;
        Activity *prev = NULL;
        while (curr->next) {
            int should_swap = 0;
            if (curr->time > curr->next->time) should_swap = 1;
            else if (curr->time == curr->next->time && strcmp(curr->label, curr->next->label) > 0) should_swap = 1;
            
            if (should_swap) {
                Activity *next = curr->next;
                curr->next = next->next;
                next->next = curr;
                if (prev) prev->next = next;
                else head = next;
                swapped = 1;
                prev = next;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    } while (swapped);
}

void clear_activities() {
    Activity *curr = head;
    while (curr) {
        Activity *next = curr->next;
        free(curr->label);
        free(curr->note);
        free(curr);
        curr = next;
    }
    head = NULL;
}

void print_activities() {
    for (Activity *curr = head; curr; curr = curr->next) {
        printf("%d %s %s\n", curr->time, curr->label, curr->note);
    }
}

void free_all() {
    clear_activities();
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        int time;
        char label[1024], note[4096];
        
        int n = sscanf(line, "%15s %d %1023s %[^\n]", cmd, &time, label, note);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 3) {
            delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %1023s", label);
            find_activity(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
            print_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_activities();
        }
    }
    free_all();
    return 0;
}