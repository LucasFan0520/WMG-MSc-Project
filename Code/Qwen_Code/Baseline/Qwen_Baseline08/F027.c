// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REM 10000
#define MAX_LEN 1000

typedef struct {
    int priority;
    char title[100];
    char note[MAX_LEN];
} Rem;

Rem rems[MAX_REM];
int rcount = 0;

int find_rem(const char *title) {
    for (int i = 0; i < rcount; i++) if (strcmp(rems[i].title, title) == 0) return i;
    return -1;
}

int cmp(const void *a, const void *b) {
    return ((Rem *)b)->priority - ((Rem *)a)->priority;
}

int main() {
    char cmd[20], title[100], note[MAX_LEN];
    int prio, delta, n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d %s", &prio, title);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (find_rem(title) == -1 && rcount < MAX_REM) {
                rems[rcount].priority = prio;
                strcpy(rems[rcount].title, title);
                strcpy(rems[rcount].note, note);
                rcount++;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            scanf("%s %d", title, &delta);
            int idx = find_rem(title);
            if (idx != -1) rems[idx].priority += delta;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", title);
            int idx = find_rem(title);
            if (idx != -1) {
                for (int i = idx; i < rcount - 1; i++) rems[i] = rems[i + 1];
                rcount--;
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            scanf("%d", &n);
            Rem temp[MAX_REM];
            memcpy(temp, rems, sizeof(Rem) * rcount);
            qsort(temp, rcount, sizeof(Rem), cmp);
            int limit = n < rcount ? n : rcount;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%d %s %s\n", rems[i].priority, rems[i].title, rems[i].note);
            }
        }
    }
    return 0;
}