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
int rc = 0;

int find_rem(const char *title) {
    for (int i = 0; i < rc; i++) {
        if (strcmp(rems[i].title, title) == 0) return i;
    }
    return -1;
}

int cmp_priority(const void *a, const void *b) {
    const Reminder *ra = (const Reminder *)a;
    const Reminder *rb = (const Reminder *)b;
    return rb->priority - ra->priority;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[256];
        char rest[2048];
        a1[0] = '\0';
        a2[0] = '\0';
        rest[0] = '\0';
        int pri;
        int n = sscanf(line, "%31s", cmd);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            n = sscanf(line, "%31s %d %255s %2047[^\n]", cmd, &pri, a1, rest);
            if (n >= 4) {
                rems = realloc(rems, (rc + 1) * sizeof(Reminder));
                if (!rems) return 1;
                rems[rc].priority = pri;
                rems[rc].title = strdup(a1);
                rems[rc].note = strdup(rest);
                rc++;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            int delta;
            n = sscanf(line, "%31s %255s %d", cmd, a1, &delta);
            if (n >= 3) {
                int ri = find_rem(a1);
                if (ri >= 0) {
                    rems[ri].priority += delta;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            n = sscanf(line, "%31s %255s", cmd, a1);
            if (n >= 2) {
                int ri = find_rem(a1);
                if (ri >= 0) {
                    free(rems[ri].title);
                    free(rems[ri].note);
                    for (int k = ri; k < rc - 1; k++) {
                        rems[k] = rems[k + 1];
                    }
                    rc--;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int topn;
            n = sscanf(line, "%31s %d", cmd, &topn);
            if (n >= 2) {
                Reminder *sorted = malloc(rc * sizeof(Reminder));
                if (!sorted && rc > 0) return 1;
                memcpy(sorted, rems, rc * sizeof(Reminder));
                qsort(sorted, rc, sizeof(Reminder), cmp_priority);
                int limit = topn < rc ? topn : rc;
                for (int i = 0; i < limit; i++) {
                    printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
                }
                free(sorted);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%d %s %s\n", rems[i].priority, rems[i].title, rems[i].note);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(rems[i].title);
        free(rems[i].note);
    }
    free(rems);
    return 0;
}