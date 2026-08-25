// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int time;
    char *label;
    char *note;
} Activity;

Activity *acts = NULL;
int act_count = 0;

int cmp(const void *a, const void *b) {
    Activity *x = (Activity*)a;
    Activity *y = (Activity*)b;
    if (x->time != y->time) return x->time - y->time;
    return strcmp(x->label, y->label);
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            int t;
            char label[100], note[1000];
            scanf("%d %s", &t, label);
            scanf(" %[^\n]", note);
            acts = realloc(acts, sizeof(Activity) * (act_count + 1));
            acts[act_count].time = t;
            acts[act_count].label = strdup(label);
            acts[act_count].note = strdup(note);
            act_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            int t;
            char label[100];
            scanf("%d %s", &t, label);
            for (int i = 0; i < act_count; ) {
                if (acts[i].time == t && strcmp(acts[i].label, label) == 0) {
                    free(acts[i].label);
                    free(acts[i].note);
                    for (int j = i; j < act_count - 1; j++) acts[j] = acts[j+1];
                    act_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char label[100];
            scanf("%s", label);
            int min_t = 2147483647;
            int idx = -1;
            for (int i = 0; i < act_count; i++) {
                if (strcmp(acts[i].label, label) == 0 && acts[i].time < min_t) {
                    min_t = acts[i].time;
                    idx = i;
                }
            }
            if (idx != -1) {
                printf("%d %s %s\n", acts[idx].time, acts[idx].label, acts[idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(acts, act_count, sizeof(Activity), cmp);
            for (int i = 0; i < act_count; i++) {
                printf("%d %s %s\n", acts[i].time, acts[i].label, acts[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < act_count; i++) {
                free(acts[i].label);
                free(acts[i].note);
            }
            act_count = 0;
        }
    }
    for (int i = 0; i < act_count; i++) {
        free(acts[i].label);
        free(acts[i].note);
    }
    free(acts);
    return 0;
}