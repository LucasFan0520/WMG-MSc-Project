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

Activity *log_head = NULL;

Activity *find_activity(const char *time, const char *label) {
    Activity *a = log_head;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0)
            return a;
        a = a->next;
    }
    return NULL;
}

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = log_head;
    log_head = a;
}

void delete_activity(const char *time, const char *label) {
    Activity *a = log_head, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else log_head = a->next;
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
    Activity *best = NULL;
    Activity *a = log_head;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!best || strcmp(a->time, best->time) < 0)
                best = a;
        }
        a = a->next;
    }
    if (!best) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s\n", best->time, best->label, best->note);
}

int compare_activity(const void *x, const void *y) {
    Activity *a = *(Activity**)x, *b = *(Activity**)y;
    int cmp = strcmp(a->time, b->time);
    if (cmp) return cmp;
    return strcmp(a->label, b->label);
}

void sort_log(void) {
    int count = 0;
    Activity *a = log_head;
    while (a) { count++; a = a->next; }
    if (count < 2) return;
    Activity **arr = malloc(count * sizeof(Activity*));
    if (!arr) return;
    int i = 0;
    a = log_head;
    while (a) { arr[i++] = a; a = a->next; }
    qsort(arr, count, sizeof(Activity*), compare_activity);
    log_head = arr[0];
    for (i = 0; i < count - 1; i++) arr[i]->next = arr[i+1];
    arr[count-1]->next = NULL;
    free(arr);
}

void clear_log(void) {
    while (log_head) {
        Activity *tmp = log_head;
        log_head = log_head->next;
        free(tmp->time);
        free(tmp->label);
        free(tmp->note);
        free(tmp);
    }
}

void print_sorted(void) {
    sort_log();
    Activity *a = log_head;
    while (a) {
        printf("%s %s %s\n", a->time, a->label, a->note);
        a = a->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (n < 3) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *note = line + strlen(cmd) + strlen(arg1) + strlen(arg2) + 3;
            while (*note == ' ') note++;
            add_activity(arg1, arg2, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_activity(arg1, arg2);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_label(arg1);
        } else if (strcmp(cmd, "SORT") == 0) {
            print_sorted();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_log();
        }
    }
    clear_log();
    return 0;
}