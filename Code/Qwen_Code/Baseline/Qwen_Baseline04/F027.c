// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

Reminder *rems = NULL;
int rcount = 0;

int cmp(const void *a, const void *b) {
    Reminder *x = (Reminder*)a;
    Reminder *y = (Reminder*)b;
    return y->priority - x->priority;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            int p;
            char title[100], note[1000];
            scanf("%d %s", &p, title);
            scanf(" %[^\n]", note);
            rems = realloc(rems, sizeof(Reminder) * (rcount + 1));
            rems[rcount].priority = p;
            rems[rcount].title = strdup(title);
            rems[rcount].note = strdup(note);
            rcount++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[100];
            int delta;
            scanf("%s %d", title, &delta);
            for (int i = 0; i < rcount; i++) {
                if (strcmp(rems[i].title, title) == 0) {
                    rems[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[100];
            scanf("%s", title);
            for (int i = 0; i < rcount; i++) {
                if (strcmp(rems[i].title, title) == 0) {
                    free(rems[i].title);
                    free(rems[i].note);
                    for (int j = i; j < rcount - 1; j++) rems[j] = rems[j+1];
                    rcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            scanf("%d", &n);
            Reminder *sorted = malloc(sizeof(Reminder) * rcount);
            memcpy(sorted, rems, sizeof(Reminder) * rcount);
            qsort(sorted, rcount, sizeof(Reminder), cmp);
            for (int i = 0; i < n && i < rcount; i++) {
                printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
            }
            free(sorted);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%d %s %s\n", rems[i].priority, rems[i].title, rems[i].note);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(rems[i].title);
        free(rems[i].note);
    }
    free(rems);
    return 0;
}