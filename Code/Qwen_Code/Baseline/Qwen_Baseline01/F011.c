// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

Draft *drafts = NULL;
int nd = 0;
Draft *discarded = NULL;
int ndisc = 0;

int find_draft(const char *t) {
    for (int i = 0; i < nd; i++)
        if (strcmp(drafts[i].title, t) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *p = line + 7 + strlen(arg1);
            if (*p == ' ') p++;
            drafts = realloc(drafts, sizeof(Draft) * (nd + 1));
            drafts[nd].title = strdup(arg1);
            drafts[nd].body = strdup(p);
            nd++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *p = line + 7 + strlen(arg1);
            if (*p == ' ') p++;
            int i = find_draft(arg1);
            if (i != -1) {
                free(drafts[i].body);
                drafts[i].body = strdup(p);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_draft(arg1);
            if (i != -1) {
                discarded = realloc(discarded, sizeof(Draft) * (ndisc + 1));
                discarded[ndisc].title = strdup(drafts[i].title);
                discarded[ndisc].body = strdup(drafts[i].body);
                ndisc++;
                free(drafts[i].title);
                free(drafts[i].body);
                for (int j = i; j < nd - 1; j++) drafts[j] = drafts[j + 1];
                nd--;
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (ndisc > 0) {
                ndisc--;
                drafts = realloc(drafts, sizeof(Draft) * (nd + 1));
                drafts[nd] = discarded[ndisc];
                nd++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_draft(arg1);
            if (i != -1) printf("%s\n", drafts[i].body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < nd; i++) printf("%s %s\n", drafts[i].title, drafts[i].body);
        }
    }
    for (int i = 0; i < nd; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    for (int i = 0; i < ndisc; i++) {
        free(discarded[i].title);
        free(discarded[i].body);
    }
    free(discarded);
    return 0;
}