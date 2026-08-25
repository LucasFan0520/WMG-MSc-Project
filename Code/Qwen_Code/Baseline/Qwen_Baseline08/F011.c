// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DRAFTS 1000
#define MAX_LEN 1000

typedef struct {
    char title[100];
    char body[MAX_LEN];
} Draft;

Draft drafts[MAX_DRAFTS];
int dcount = 0;
Draft discarded[MAX_DRAFTS];
int dis_count = 0;

int find_draft(const char *title) {
    for (int i = 0; i < dcount; i++) if (strcmp(drafts[i].title, title) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], title[100], body[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "CREATE") == 0) {
            scanf("%s", title);
            fgets(body, MAX_LEN, stdin);
            body[strcspn(body, "\n")] = 0;
            if (body[0] == ' ') memmove(body, body + 1, strlen(body));
            if (find_draft(title) == -1 && dcount < MAX_DRAFTS) {
                strcpy(drafts[dcount].title, title);
                strcpy(drafts[dcount].body, body);
                dcount++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            scanf("%s", title);
            fgets(body, MAX_LEN, stdin);
            body[strcspn(body, "\n")] = 0;
            if (body[0] == ' ') memmove(body, body + 1, strlen(body));
            int idx = find_draft(title);
            if (idx != -1) strcpy(drafts[idx].body, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            scanf("%s", title);
            int idx = find_draft(title);
            if (idx != -1) {
                if (dis_count < MAX_DRAFTS) {
                    strcpy(discarded[dis_count].title, drafts[idx].title);
                    strcpy(discarded[dis_count].body, drafts[idx].body);
                    dis_count++;
                }
                for (int i = idx; i < dcount - 1; i++) drafts[i] = drafts[i + 1];
                dcount--;
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (dis_count > 0 && dcount < MAX_DRAFTS) {
                dis_count--;
                strcpy(drafts[dcount].title, discarded[dis_count].title);
                strcpy(drafts[dcount].body, discarded[dis_count].body);
                dcount++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int idx = find_draft(title);
            if (idx != -1) printf("%s\n", drafts[idx].body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < dcount; i++) printf("%s %s\n", drafts[i].title, drafts[i].body);
        }
    }
    return 0;
}