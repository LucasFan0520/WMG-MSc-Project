// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

Draft drafts[1000];
int dc = 0;
Draft discarded[1000];
int disc_c = 0;

int find_title(char *title) {
    for (int i = 0; i < dc; i++) if (strcmp(drafts[i].title, title) == 0) return i;
    return -1;
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
        
        if (strcmp(cmd, "CREATE") == 0) {
            char title[1000];
            sscanf(rest, "%s", title);
            char *body = rest + strlen(title);
            while (*body == ' ') body++;
            if (find_title(title) == -1) {
                drafts[dc].title = strdup(title);
                drafts[dc].body = strdup(body);
                dc++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[1000];
            sscanf(rest, "%s", title);
            char *body = rest + strlen(title);
            while (*body == ' ') body++;
            int idx = find_title(title);
            if (idx != -1) {
                free(drafts[idx].body);
                drafts[idx].body = strdup(body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[1000];
            sscanf(rest, "%s", title);
            int idx = find_title(title);
            if (idx != -1) {
                discarded[disc_c].title = drafts[idx].title;
                discarded[disc_c].body = drafts[idx].body;
                disc_c++;
                for (int i = idx; i < dc - 1; i++) drafts[i] = drafts[i+1];
                dc--;
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (disc_c > 0) {
                disc_c--;
                drafts[dc].title = discarded[disc_c].title;
                drafts[dc].body = discarded[disc_c].body;
                dc++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[1000];
            sscanf(rest, "%s", title);
            int idx = find_title(title);
            if (idx != -1) printf("%s %s\n", drafts[idx].title, drafts[idx].body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < dc; i++) printf("%s %s\n", drafts[i].title, drafts[i].body);
        }
    }
    for (int i = 0; i < dc; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    for (int i = 0; i < disc_c; i++) {
        free(discarded[i].title);
        free(discarded[i].body);
    }
    return 0;
}