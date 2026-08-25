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
int count = 0;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (tail) tail->next = a;
    else head = a;
    tail = a;
    count++;
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->time, time) == 0 && strcmp(curr->label, label) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->time);
            free(curr->label);
            free(curr->note);
            free(curr);
            count--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_activity(const char *label) {
    Activity *a = head;
    Activity *earliest = NULL;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0) {
                earliest = a;
            }
        }
        a = a->next;
    }
    if (!earliest) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
}

int compare_activity(const void *p1, const void *p2) {
    Activity *a1 = *(Activity **)p1;
    Activity *a2 = *(Activity **)p2;
    int cmp = strcmp(a1->time, a2->time);
    if (cmp != 0) return cmp;
    return strcmp(a1->label, a2->label);
}

void sort_activities(void) {
    if (count < 2) return;
    Activity **arr = malloc(count * sizeof(Activity *));
    if (!arr) return;
    Activity *a = head;
    for (int i = 0; i < count; i++) {
        arr[i] = a;
        a = a->next;
    }
    qsort(arr, count, sizeof(Activity *), compare_activity);
    for (int i = 0; i < count; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_activities(void) {
    Activity *a = head;
    while (a) {
        Activity *tmp = a;
        a = a->next;
        free(tmp->time);
        free(tmp->label);
        free(tmp->note);
        free(tmp);
    }
    head = tail = NULL;
    count = 0;
}

void free_all(void) {
    clear_activities();
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char time[64], label[64];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%63s %63s", time, label) == 2) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    add_activity(time, label, note);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char time[64], label[64];
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%63s %63s", time, label) == 2) {
                delete_activity(time, label);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_activity(p);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_activities();
        }
    }
    free_all();
    return 0;
}