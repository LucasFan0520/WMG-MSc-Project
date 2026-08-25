// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Activity {
    char *time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *log_head = NULL;
Activity *log_tail = NULL;
int log_count = 0;

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    a->time = safe_strdup(time);
    a->label = safe_strdup(label);
    a->note = safe_strdup(note);
    a->next = NULL;
    if (log_tail) {
        log_tail->next = a;
        log_tail = a;
    } else {
        log_head = log_tail = a;
    }
    log_count++;
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL, *cur = log_head;
    while (cur) {
        if (strcmp(cur->time, time) == 0 && strcmp(cur->label, label) == 0) {
            if (prev) prev->next = cur->next;
            else log_head = cur->next;
            if (cur == log_tail) log_tail = prev;
            free(cur->time);
            free(cur->label);
            free(cur->note);
            free(cur);
            log_count--;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_label(const char *label) {
    Activity *cur = log_head;
    Activity *earliest = NULL;
    while (cur) {
        if (strcmp(cur->label, label) == 0) {
            if (!earliest || strcmp(cur->time, earliest->time) < 0)
                earliest = cur;
        }
        cur = cur->next;
    }
    if (!earliest) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
}

int compare_activity(const void *a, const void *b) {
    Activity *aa = *(Activity **)a;
    Activity *bb = *(Activity **)b;
    int cmp = strcmp(aa->time, bb->time);
    if (cmp != 0) return cmp;
    return strcmp(aa->label, bb->label);
}

void sort_and_print(void) {
    if (log_count == 0) return;
    Activity **arr = malloc(log_count * sizeof(Activity *));
    if (!arr) return;
    Activity *cur = log_head;
    for (int i = 0; i < log_count; i++) {
        arr[i] = cur;
        cur = cur->next;
    }
    qsort(arr, log_count, sizeof(Activity *), compare_activity);
    for (int i = 0; i < log_count; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_log(void) {
    Activity *cur = log_head;
    while (cur) {
        Activity *next = cur->next;
        free(cur->time);
        free(cur->label);
        free(cur->note);
        free(cur);
        cur = next;
    }
    log_head = log_tail = NULL;
    log_count = 0;
}

void free_all(void) {
    clear_log();
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            char time[512], label[512];
            int n2 = sscanf(line, "%*s %511s %511s", time, label);
            if (n2 < 2) continue;
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            add_activity(time, label, p);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 3) {
            delete_activity(arg1, arg2);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_label(arg1);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_and_print();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_log();
        }
    }
    free_all();
    return 0;
}