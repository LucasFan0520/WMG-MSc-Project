/* F007.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Activity {
    char *time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *log_head = NULL;
int activity_count = 0;

void free_activity(Activity *a) {
    free(a->time);
    free(a->label);
    free(a->note);
    free(a);
}

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = log_head;
    log_head = a;
    activity_count++;
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL, *curr = log_head;
    while (curr) {
        if (strcmp(curr->time, time) == 0 && strcmp(curr->label, label) == 0) {
            if (prev) prev->next = curr->next;
            else log_head = curr->next;
            free_activity(curr);
            activity_count--;
            curr = prev ? prev->next : log_head;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_label(const char *label) {
    Activity *best = NULL;
    Activity *curr = log_head;
    while (curr) {
        if (strcmp(curr->label, label) == 0) {
            if (!best || strcmp(curr->time, best->time) < 0) best = curr;
        }
        curr = curr->next;
    }
    if (!best) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s\n", best->time, best->label, best->note);
}

int compare_activity(const void *a, const void *b) {
    Activity *A = *(Activity **)a;
    Activity *B = *(Activity **)b;
    int cmp = strcmp(A->time, B->time);
    if (cmp != 0) return cmp;
    return strcmp(A->label, B->label);
}

void sort_and_print(void) {
    if (activity_count == 0) return;
    Activity **arr = malloc(activity_count * sizeof(Activity *));
    Activity *curr = log_head;
    int i = 0;
    while (curr) {
        arr[i++] = curr;
        curr = curr->next;
    }
    qsort(arr, activity_count, sizeof(Activity *), compare_activity);
    for (i = 0; i < activity_count; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_log(void) {
    while (log_head) {
        Activity *tmp = log_head;
        log_head = log_head->next;
        free_activity(tmp);
    }
    activity_count = 0;
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char time[256], label[256], note[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", time, label) != 2) continue;
            p += strlen(time) + strlen(label) + 2;
            while (*p == ' ') p++;
            add_activity(time, label, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", time, label) == 2) delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", label) == 1) find_label(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_and_print();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_log();
        }
    }

    clear_log();
    return 0;
}