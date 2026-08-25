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
int count = 0;

int cmp(const void *a, const void *b) {
    Activity *x = (Activity*)a;
    Activity *y = (Activity*)b;
    if (x->time != y->time) {
        return x->time - y->time;
    }
    return strcmp(x->label, y->label);
}

int main() {
    char cmd[20];
    char label[1000];
    char note[10000];
    int t;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d %s", &t, label);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = 0;
            acts = realloc(acts, sizeof(Activity) * (count + 1));
            acts[count].time = t;
            acts[count].label = strdup(label);
            acts[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d %s", &t, label);
            for (int i = 0; i < count; ) {
                if (acts[i].time == t && strcmp(acts[i].label, label) == 0) {
                    free(acts[i].label);
                    free(acts[i].note);
                    for (int j = i; j < count - 1; j++) {
                        acts[j] = acts[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", label);
            int min_t = 2147483647;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(acts[i].label, label) == 0 && acts[i].time < min_t) {
                    min_t = acts[i].time;
                    idx = i;
                }
            }
            if (idx != -1) {
                printf("%d %s %s\n", acts[idx].time, acts[idx].label, acts[idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(acts, count, sizeof(Activity), cmp);
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", acts[i].time, acts[i].label, acts[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(acts[i].label);
                free(acts[i].note);
            }
            count = 0;
        }
    }

    for (int i = 0; i < count; i++) {
        free(acts[i].label);
        free(acts[i].note);
    }
    free(acts);

    return 0;
}