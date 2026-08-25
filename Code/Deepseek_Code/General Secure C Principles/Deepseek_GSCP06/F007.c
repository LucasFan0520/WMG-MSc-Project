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
    Activity *a = malloc(sizeof(Activity));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = head;
    head = a;
}

void delete_activity(const char *time, const char *label) {
    Activity *cur = head, *prev = NULL;
    while (cur) {
        if (strcmp(cur->time, time) == 0 && strcmp(cur->label, label) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->time);
            free(cur->label);
            free(cur->note);
            free(cur);
            // only remove first match? The spec says "removes matching activities" (plural), so we continue.
            // We need to restart? We can continue from prev.
            // Easiest: continue with prev->next if prev else head.
            if (prev) cur = prev->next;
            else cur = head;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void find_earliest(const char *label) {
    Activity *best = NULL;
    Activity *cur = head;
    while (cur) {
        if (strcmp(cur->label, label) == 0) {
            if (!best || strcmp(cur->time, best->time) < 0) best = cur;
        }
        cur = cur->next;
    }
    if (best) printf("%s %s %s\n", best->time, best->label, best->note);
}

int compare_activities(const void *a, const void *b) {
    Activity *A = *(Activity **)a;
    Activity *B = *(Activity **)b;
    int cmp = strcmp(A->time, B->time);
    if (cmp != 0) return cmp;
    return strcmp(A->label, B->label);
}

void sort_and_print(void) {
    int count = 0;
    Activity *cur = head;
    while (cur) { count++; cur = cur->next; }
    if (count == 0) return;
    Activity **arr = malloc(count * sizeof(Activity *));
    cur = head;
    for (int i = 0; i < count; i++) { arr[i] = cur; cur = cur->next; }
    qsort(arr, count, sizeof(Activity *), compare_activities);
    for (int i = 0; i < count; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_activities(void) {
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

void cleanup_activities(void) {
    clear_activities();
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], rest[2048];
        int n = sscanf(line, "%31s %255s %255s %2047s", cmd, arg1, arg2, rest);
        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            char *note = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            add_activity(arg1, arg2, note);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 3) {
            delete_activity(arg1, arg2);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_earliest(arg1);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_and_print();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_activities();
        }
    }
    cleanup_activities();
    return 0;
}