// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *body; } Draft;
Draft *drafts = 0; int n = 0, cap = 0;
Draft last_discard = {0, 0};

int find(char *title) {
    for (int i=0; i<n; i++) if (!strcmp(drafts[i].title, title)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], title[256], rest[1700];
        rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "CREATE") || !strcmp(cmd, "REVISE")) sscanf(line, "%*s %s %[^\n]", title, rest);
        else sscanf(line, "%*s %s", title);

        if (!strcmp(cmd, "CREATE")) {
            if (find(title) < 0) {
                if (n == cap) { cap = cap ? cap*2 : 4; drafts = realloc(drafts, cap*sizeof(Draft)); }
                drafts[n].title = strdup(title); drafts[n].body = strdup(rest); n++;
            }
        } else if (!strcmp(cmd, "REVISE")) {
            int i = find(title);
            if (i >= 0) { free(drafts[i].body); drafts[i].body = strdup(rest); }
        } else if (!strcmp(cmd, "DISCARD")) {
            int i = find(title);
            if (i >= 0) {
                if (last_discard.title) { free(last_discard.title); free(last_discard.body); }
                last_discard.title = drafts[i].title; last_discard.body = drafts[i].body;
                for (int j=i; j<n-1; j++) drafts[j] = drafts[j+1]; n--;
            }
        } else if (!strcmp(cmd, "RESTORE")) {
            if (last_discard.title) {
                if (n == cap) { cap = cap ? cap*2 : 4; drafts = realloc(drafts, cap*sizeof(Draft)); }
                drafts[n++] = last_discard; last_discard.title = 0; last_discard.body = 0;
            }
        } else if (!strcmp(cmd, "FIND")) {
            int i = find(title);
            if (i >= 0) printf("%s %s\n", drafts[i].title, drafts[i].body);
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i=0; i<n; i++) printf("%s %s\n", drafts[i].title, drafts[i].body);
        }
    }
    return 0;
}