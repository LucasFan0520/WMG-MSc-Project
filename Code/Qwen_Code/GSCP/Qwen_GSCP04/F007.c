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
    a->time = time;
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = head;
    head = a;
}

void delete_activity(int time, const char *label) {
    Activity *curr = head, *prev = NULL;
    while (curr) {
        if (curr->time == time && strcmp(curr->label, label) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->label);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_activity(const char *label) {
    Activity *curr = head, *earliest = NULL;
    while (curr) {
        if (strcmp(curr->label, label) == 0) {
            if (!earliest || curr->time < earliest->time) earliest = curr;
        }
        curr = curr->next;
    }
    if (earliest) printf("%d %s %s\n", earliest->time, earliest->label, earliest->note);
}

void sort_activities() {
    int n = 0;
    Activity *curr = head;
    while (curr) { n++; curr = curr->next; }
    if (n == 0) return;
    
    Activity **arr = malloc(n * sizeof(Activity*));
    curr = head;
    for (int i = 0; i < n; i++) { arr[i] = curr; curr = curr->next; }
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int swap = 0;
            if (arr[j]->time > arr[j+1]->time) swap = 1;
            else if (arr[j]->time == arr[j+1]->time && strcmp(arr[j]->label, arr[j+1]->label) > 0) swap = 1;
            if (swap) {
                Activity *tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
    for (int i = 0; i < n; i++) printf("%d %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    free(arr);
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], label[256], note[4096] = {0};
        int time;
        int n = sscanf(line, "%31s %d %255s %4095[^\n]", cmd, &time, label, note);
        
        if (strcmp(cmd, "ADD") == 0 && n >= 3) add_activity(time, label, note);
        else if (strcmp(cmd, "DELETE") == 0 && n >= 3) delete_activity(time, label);
        else if (strcmp(cmd, "FIND") == 0 && n >= 2) find_activity(label);
        else if (strcmp(cmd, "SORT") == 0) sort_activities();
        else if (strcmp(cmd, "CLEAR") == 0) clear_activities();
    }
    clear_activities();
    return 0;
}