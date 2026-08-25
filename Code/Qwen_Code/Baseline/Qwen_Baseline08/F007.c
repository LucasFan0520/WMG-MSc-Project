// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACTS 10000
#define MAX_LEN 1000

typedef struct {
    int time;
    char label[100];
    char note[MAX_LEN];
} Act;

Act acts[MAX_ACTS];
int count = 0;

int cmp(const void *a, const void *b) {
    Act *x = (Act *)a, *y = (Act *)b;
    if (x->time != y->time) return x->time - y->time;
    return strcmp(x->label, y->label);
}

int main() {
    char cmd[20], label[100], note[MAX_LEN];
    int time;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d %s", &time, label);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (count < MAX_ACTS) {
                acts[count].time = time;
                strcpy(acts[count].label, label);
                strcpy(acts[count].note, note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d %s", &time, label);
            for (int i = 0; i < count; ) {
                if (acts[i].time == time && strcmp(acts[i].label, label) == 0) {
                    for (int j = i; j < count - 1; j++) acts[j] = acts[j + 1];
                    count--;
                } else i++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", label);
            int min_time = 2147483647, idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(acts[i].label, label) == 0 && acts[i].time < min_time) {
                    min_time = acts[i].time;
                    idx = i;
                }
            }
            if (idx != -1) printf("%d %s %s\n", acts[idx].time, acts[idx].label, acts[idx].note);
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(acts, count, sizeof(Act), cmp);
            for (int i = 0; i < count; i++) printf("%d %s %s\n", acts[i].time, acts[i].label, acts[i].note);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            count = 0;
        }
    }
    return 0;
}