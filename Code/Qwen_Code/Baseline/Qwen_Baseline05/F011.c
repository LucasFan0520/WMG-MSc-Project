// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 100
#define MAX_BODY 10000

typedef struct {
    char title[MAX_TITLE];
    char body[MAX_BODY];
} Draft;

Draft drafts[1000];
int draft_count = 0;

Draft discarded[1000];
int discarded_count = 0;

int find_draft(const char *title) {
    for (int i = 0; i < draft_count; i++) {
        if (strcmp(drafts[i].title, title) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], title[MAX_TITLE], body[MAX_BODY];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "CREATE") == 0) {
            scanf("%s", title);
            fgets(body, MAX_BODY, stdin);
            if (body[0] == ' ') memmove(body, body + 1, strlen(body));
            if (body[strlen(body) - 1] == '\n') body[strlen(body) - 1] = '\0';
            
            strcpy(drafts[draft_count].title, title);
            strcpy(drafts[draft_count].body, body);
            draft_count++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            scanf("%s", title);
            fgets(body, MAX_BODY, stdin);
            if (body[0] == ' ') memmove(body, body + 1, strlen(body));
            if (body[strlen(body) - 1] == '\n') body[strlen(body) - 1] = '\0';
            
            int idx = find_draft(title);
            if (idx != -1) {
                strcpy(drafts[idx].body, body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            scanf("%s", title);
            int idx = find_draft(title);
            if (idx != -1) {
                strcpy(discarded[discarded_count].title, drafts[idx].title);
                strcpy(discarded[discarded_count].body, drafts[idx].body);
                discarded_count++;
                
                for (int i = idx; i < draft_count - 1; i++) {
                    drafts[i] = drafts[i + 1];
                }
                draft_count--;
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (discarded_count > 0) {
                discarded_count--;
                strcpy(drafts[draft_count].title, discarded[discarded_count].title);
                strcpy(drafts[draft_count].body, discarded[discarded_count].body);
                draft_count++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int idx = find_draft(title);
            if (idx != -1) {
                printf("%s %s\n", drafts[idx].title, drafts[idx].body);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < draft_count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }
    
    return 0;
}