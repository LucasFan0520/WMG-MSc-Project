// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *body; } Draft;
Draft d[1000]; int nd = 0;
Draft hist[1000]; int nh = 0;

int find(char *title) {
    for (int i = 0; i < nd; i++) if (!strcmp(d[i].title, title)) return i;
    return -1;
}

int main() {
    char cmd[20], title[100], body[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "CREATE") || !strcmp(cmd, "REVISE")) {
            scanf("%s", title); fgets(body, sizeof(body), stdin);
            body[strcspn(body, "\n")] = 0;
            if (body[0] == ' ') memmove(body, body + 1, strlen(body));
            int idx = find(title);
            if (!strcmp(cmd, "CREATE")) {
                if (idx == -1) { d[nd].title = strdup(title); d[nd++].body = strdup(body); }
            } else {
                if (idx != -1) { free(d[idx].body); d[idx].body = strdup(body); }
            }
        } else if (!strcmp(cmd, "DISCARD")) {
            scanf("%s", title); int idx = find(title);
            if (idx != -1) {
                hist[nh].title = strdup(d[idx].title); hist[nh++].body = strdup(d[idx].body);
                free(d[idx].title); free(d[idx].body);
                for (int i = idx; i < nd - 1; i++) d[i] = d[i + 1];
                nd--;
            }
        } else if (!strcmp(cmd, "RESTORE")) {
            if (nh > 0) {
                nh--;
                d[nd].title = hist[nh].title; d[nd++].body = hist[nh].body;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", title); int idx = find(title);
            if (idx != -1) printf("%s %s\n", d[idx].title, d[idx].body);
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < nd; i++) printf("%s %s\n", d[i].title, d[i].body);
        }
    }
    for (int i = 0; i < nd; i++) { free(d[i].title); free(d[i].body); }
    for (int i = 0; i < nh; i++) { free(hist[i].title); free(hist[i].body); }
    return 0;
}