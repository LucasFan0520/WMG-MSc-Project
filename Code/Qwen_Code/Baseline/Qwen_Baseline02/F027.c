// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int priority; char *title; char *note; int order; } Rem;
Rem rems[10000]; int nrems = 0; int order_cnt = 0;

int find_rem(char *title) {
    for (int i = 0; i < nrems; i++) if (!strcmp(rems[i].title, title)) return i;
    return -1;
}

int main() {
    char cmd[20], title[100], note[1000]; int priority, delta, n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%d", &priority);
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            char *space = strchr(title, ' ');
            if (space) {
                *space = 0;
                char *nt = space + 1;
                while(*nt == ' ') nt++;
                rems[nrems].priority = priority;
                rems[nrems].title = strdup(title);
                rems[nrems].note = strdup(nt);
                rems[nrems++].order = order_cnt++;
            }
        } else if (!strcmp(cmd, "BUMP")) {
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            char *space = strrchr(title, ' ');
            if (space) {
                *space = 0;
                delta = atoi(space + 1);
                int idx = find_rem(title);
                if (idx != -1) rems[idx].priority += delta;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            int idx = find_rem(title);
            if (idx != -1) {
                free(rems[idx].title);
                free(rems[idx].note);
                for (int i = idx; i < nrems - 1; i++) rems[i] = rems[i + 1];
                nrems--;
            }
        } else if (!strcmp(cmd, "TOP")) {
            scanf("%d", &n);
            Rem *sorted = malloc(sizeof(Rem) * nrems);
            memcpy(sorted, rems, sizeof(Rem) * nrems);
            for (int i = 0; i < nrems - 1; i++) {
                for (int j = 0; j < nrems - i - 1; j++) {
                    int swap = 0;
                    if (sorted[j].priority < sorted[j+1].priority) swap = 1;
                    else if (sorted[j].priority == sorted[j+1].priority && sorted[j].order > sorted[j+1].order) swap = 1;
                    if (swap) { Rem t = sorted[j]; sorted[j] = sorted[j+1]; sorted[j+1] = t; }
                }
            }
            for (int i = 0; i < n && i < nrems; i++) {
                printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
            }
            free(sorted);
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < nrems; i++) {
                printf("%d %s %s\n", rems[i].priority, rems[i].title, rems[i].note);
            }
        }
    }
    for (int i = 0; i < nrems; i++) {
        free(rems[i].title);
        free(rems[i].note);
    }
    return 0;
}