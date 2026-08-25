// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

Reminder rem[10000];
int rc = 0;

int cmp(const void *a, const void *b) {
    Reminder *x = (Reminder *)a;
    Reminder *y = (Reminder *)b;
    return y->priority - x->priority;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            int p;
            char title[1000];
            sscanf(rest, "%d %s", &p, title);
            char *note = rest;
            while (*note && *note != ' ') note++;
            while (*note == ' ') note++;
            while (*note && *note != ' ') note++;
            while (*note == ' ') note++;
            rem[rc].priority = p;
            rem[rc].title = strdup(title);
            rem[rc].note = strdup(note);
            rc++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[1000];
            int delta;
            sscanf(rest, "%s %d", title, &delta);
            for (int i = 0; i < rc; i++) {
                if (strcmp(rem[i].title, title) == 0) {
                    rem[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (int i = 0; i < rc; i++) {
                if (strcmp(rem[i].title, rest) == 0) {
                    free(rem[i].title);
                    free(rem[i].note);
                    for (int j = i; j < rc - 1; j++) rem[j] = rem[j+1];
                    rc--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            sscanf(rest, "%d", &n);
            Reminder *sorted = malloc(sizeof(Reminder) * rc);
            memcpy(sorted, rem, sizeof(Reminder) * rc);
            qsort(sorted, rc, sizeof(Reminder), cmp);
            int limit = n < rc ? n : rc;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
            }
            free(sorted);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%d %s %s\n", rem[i].priority, rem[i].title, rem[i].note);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(rem[i].title);
        free(rem[i].note);
    }
    return 0;
}