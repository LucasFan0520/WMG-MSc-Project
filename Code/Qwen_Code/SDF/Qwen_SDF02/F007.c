// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Activity {
    int time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *activities = NULL;

void add_activity(int time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    a->time = time;
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = activities;
    activities = a;
}

int cmp_activity(const void *a, const void *b) {
    const Activity *aa = *(const Activity **)a;
    const Activity *bb = *(const Activity **)b;
    if (aa->time != bb->time) return aa->time - bb->time;
    return strcmp(aa->label, bb->label);
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            int time;
            char label[256];
            int n = 0;
            if (sscanf(rest, "%d %255s%n", &time, label, &n) != 2) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int time;
            char label[256];
            if (sscanf(rest, "%d %255s", &time, label) != 2) continue;
            Activity *prev = NULL;
            for (Activity *a = activities; a; ) {
                if (a->time == time && strcmp(a->label, label) == 0) {
                    Activity *to_del = a;
                    a = a->next;
                    if (prev) prev->next = a;
                    else activities = a;
                    free(to_del->label);
                    free(to_del->note);
                    free(to_del);
                } else {
                    prev = a;
                    a = a->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char label[256];
            if (sscanf(rest, "%255s", label) != 1) continue;
            Activity *earliest = NULL;
            for (Activity *a = activities; a; a = a->next) {
                if (strcmp(a->label, label) == 0) {
                    if (!earliest || a->time < earliest->time) earliest = a;
                }
            }
            if (earliest) printf("%d %s %s\n", earliest->time, earliest->label, earliest->note);
        } else if (strcmp(cmd, "SORT") == 0) {
            size_t count = 0;
            for (Activity *a = activities; a; a = a->next) count++;
            Activity **arr = malloc(count * sizeof(Activity*));
            if (!arr) continue;
            size_t idx = 0;
            for (Activity *a = activities; a; a = a->next) arr[idx++] = a;
            qsort(arr, count, sizeof(Activity*), cmp_activity);
            for (size_t i = 0; i < count; i++)
                printf("%d %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
            free(arr);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            while (activities) {
                Activity *a = activities;
                activities = activities->next;
                free(a->label);
                free(a->note);
                free(a);
            }
        }
    }
    while (activities) {
        Activity *a = activities;
        activities = activities->next;
        free(a->label);
        free(a->note);
        free(a);
    }
    return 0;
}