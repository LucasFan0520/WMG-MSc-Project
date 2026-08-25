// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int pri;
    char title[100];
    char note[1000];
} Rem;

Rem rems[1000];
int rn = 0;

int cmp(const void *a, const void *b) {
    return ((Rem *)b)->pri - ((Rem *)a)->pri;
}

int main() {
    char cmd[20], title[100], note[1000];
    int pri, delta, n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%d %s", &pri, title);
            fgets(note, 1000, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            rems[rn].pri = pri;
            strcpy(rems[rn].title, title);
            strcpy(rems[rn].note, note);
            rn++;
        } else if (!strcmp(cmd, "BUMP")) {
            scanf("%s %d", title, &delta);
            for (int i = 0; i < rn; i++) {
                if (!strcmp(rems[i].title, title)) {
                    rems[i].pri += delta;
                    break;
                }
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s", title);
            for (int i = 0; i < rn; i++) {
                if (!strcmp(rems[i].title, title)) {
                    for (int j = i; j < rn - 1; j++) {
                        rems[j] = rems[j + 1];
                    }
                    rn--;
                    break;
                }
            }
        } else if (!strcmp(cmd, "TOP")) {
            scanf("%d", &n);
            Rem tmp[1000];
            for (int i = 0; i < rn; i++) {
                tmp[i] = rems[i];
            }
            qsort(tmp, rn, sizeof(Rem), cmp);
            int limit = n < rn ? n : rn;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", tmp[i].pri, tmp[i].title, tmp[i].note);
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < rn; i++) {
                printf("%d %s %s\n", rems[i].pri, rems[i].title, rems[i].note);
            }
        }
    }
    return 0;
}