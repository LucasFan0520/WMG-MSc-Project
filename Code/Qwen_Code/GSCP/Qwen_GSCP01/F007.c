// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Activity {
    long time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *head = NULL;

void add_activity(long time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) exit(EXIT_FAILURE);
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

void delete_activity(long time, const char *label) {
    Activity *curr = head;
    Activity *prev = NULL;
    while (curr) {
        if (curr->time == time && strcmp(curr->label, label) == 0) {
            Activity *to_delete = curr;
            if (prev) prev->next = curr->next;
            else head = curr->next;
            curr = curr->next;
            free(to_delete->label);
            free(to_delete->note);
            free(to_delete);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_activity(const char *label) {
    Activity *curr = head;
    Activity *earliest = NULL;
    while (curr) {
        if (strcmp(curr->label, label) == 0) {
            if (!earliest || curr->time < earliest->time) {
                earliest = curr;
            }
        }
        curr = curr->next;
    }
    if (earliest) {
        printf("%ld %s %s\n", earliest->time, earliest->label, earliest->note);
    }
}

int cmp_activity(const void *a, const void *b) {
    const Activity *ia = *(const Activity **)a;
    const Activity *ib = *(const Activity **)b;
    if (ia->time != ib->time) return (ia->time > ib->time) - (ia->time < ib->time);
    return strcmp(ia->label, ib->label);
}

void sort_activities() {
    int count = 0;
    Activity *curr = head;
    while (curr) {
        count++;
        curr = curr->next;
    }
    if (count == 0) return;
    
    Activity **arr = malloc(count * sizeof(Activity *));
    if (!arr) exit(EXIT_FAILURE);
    
    curr = head;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }
    
    qsort(arr, count, sizeof(Activity *), cmp_activity);
    
    for (int i = 0; i < count; i++) {
        printf("%ld %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
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

void free_all() {
    clear_activities();
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        long time_val = 0;
        char label[256] = {0};
        char note[3500] = {0};
        
        int matched = sscanf(line, "%31s %ld %255s %[^\n]", cmd, &time_val, label, note);
        
        if (strcmp(cmd, "ADD") == 0 && matched == 4) {
            add_activity(time_val, label, note);
        } else if (strcmp(cmd, "DELETE") == 0 && matched >= 3) {
            delete_activity(time_val, label);
        } else if (strcmp(cmd, "FIND") == 0 && matched >= 2) {
            find_activity(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_activities();
        }
    }
    free_all();
    return 0;
}