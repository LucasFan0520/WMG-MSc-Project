// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

Draft *drafts = NULL;
int dcount = 0;

Draft *discard_stack = NULL;
int stack_count = 0;

int find_draft(const char *title) {
    for (int i = 0; i < dcount; i++) {
        if (strcmp(drafts[i].title, title) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "CREATE") == 0) {
            char title[100], body[1000];
            scanf("%s", title);
            scanf(" %[^\n]", body);
            drafts = realloc(drafts, sizeof(Draft) * (dcount + 1));
            drafts[dcount].title = strdup(title);
            drafts[dcount].body = strdup(body);
            dcount++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[100], body[1000];
            scanf("%s", title);
            scanf(" %[^\n]", body);
            int idx = find_draft(title);
            if (idx != -1) {
                free(drafts[idx].body);
                drafts[idx].body = strdup(body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[100];
            scanf("%s", title);
            int idx = find_draft(title);
            if (idx != -1) {
                discard_stack = realloc(discard_stack, sizeof(Draft) * (stack_count + 1));
                discard_stack[stack_count].title = strdup(drafts[idx].title);
                discard_stack[stack_count].body = strdup(drafts[idx].body);
                stack_count++;
                
                free(drafts[idx].title);
                free(drafts[idx].body);
                for (int j = idx; j < dcount - 1; j++) drafts[j] = drafts[j+1];
                dcount--;
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (stack_count > 0) {
                stack_count--;
                drafts = realloc(drafts, sizeof(Draft) * (dcount + 1));
                drafts[dcount] = discard_stack[stack_count];
                dcount++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[100];
            scanf("%s", title);
            int idx = find_draft(title);
            if (idx != -1) {
                printf("%s %s\n", drafts[idx].title, drafts[idx].body);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < dcount; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }
    for (int i = 0; i < dcount; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    for (int i = 0; i < stack_count; i++) {
        free(discard_stack[i].title);
        free(discard_stack[i].body);
    }
    free(discard_stack);
    return 0;
}