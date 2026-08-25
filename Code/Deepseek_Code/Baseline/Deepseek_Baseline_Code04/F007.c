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

Activity *log = NULL;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = log;
    log = a;
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL, *cur = log;
    while (cur) {
        if (strcmp(cur->time, time) == 0 && strcmp(cur->label, label) == 0) {
            if (prev) prev->next = cur->next;
            else log = cur->next;
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

Activity *find_earliest(const char *label) {
    Activity *best = NULL;
    Activity *cur = log;
    while (cur) {
        if (strcmp(cur->label, label) == 0) {
            if (!best || strcmp(cur->time, best->time) < 0) best = cur;
        }
        cur = cur->next;
    }
    return best;
}

int compare_activity(const void *a, const void *b) {
    Activity *aa = *(Activity **)a;
    Activity *bb = *(Activity **)b;
    int cmp = strcmp(aa->time, bb->time);
    if (cmp != 0) return cmp;
    return strcmp(aa->label, bb->label);
}

void sort_and_print(void) {
    Activity *cur = log;
    int count = 0;
    while (cur) { count++; cur = cur->next; }
    if (count == 0) return;
    Activity **arr = malloc(count * sizeof(Activity *));
    cur = log;
    for (int i = 0; i < count; i++) {
        arr[i] = cur;
        cur = cur->next;
    }
    qsort(arr, count, sizeof(Activity *), compare_activity);
    for (int i = 0; i < count; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_log(void) {
    while (log) {
        Activity *tmp = log;
        log = log->next;
        free(tmp->time);
        free(tmp->label);
        free(tmp->note);
        free(tmp);
    }
}

void free_log(void) {
    clear_log();
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char time[256], label[256];
        int n = sscanf(line, "%s %s %s", cmd, time, label);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 3) continue;
            char *p = line + strlen(cmd) + 1 + strlen(time) + 1 + strlen(label) + 1;
            while (*p == ' ') p++;
            if (*p) add_activity(time, label, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 3) continue;
            delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            Activity *a = find_earliest(label);
            if (a) printf("%s\n", a->note);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_and_print();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_log();
        }
    }
    free_log();
    return 0;
}