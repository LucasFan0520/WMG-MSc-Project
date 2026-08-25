// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

Draft *drafts = NULL;
int d_count = 0;

Draft *trash = NULL;
int t_count = 0;

int find_title(const char *title) {
    for (int i = 0; i < d_count; i++) {
        if (strcmp(drafts[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[20];
    char title[1000];
    char body[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "CREATE") == 0) {
            scanf("%s", title);
            fgets(body, sizeof(body), stdin);
            if (body[0] == ' ') {
                memmove(body, body + 1, strlen(body));
            }
            body[strcspn(body, "\n")] = 0;
            int idx = find_title(title);
            if (idx != -1) {
                free(drafts[idx].body);
                drafts[idx].body = strdup(body);
            } else {
                drafts = realloc(drafts, sizeof(Draft) * (d_count + 1));
                drafts[d_count].title = strdup(title);
                drafts[d_count].body = strdup(body);
                d_count++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            scanf("%s", title);
            fgets(body, sizeof(body), stdin);
            if (body[0] == ' ') {
                memmove(body, body + 1, strlen(body));
            }
            body[strcspn(body, "\n")] = 0;
            int idx = find_title(title);
            if (idx != -1) {
                free(drafts[idx].body);
                drafts[idx].body = strdup(body);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            scanf("%s", title);
            int idx = find_title(title);
            if (idx != -1) {
                trash = realloc(trash, sizeof(Draft) * (t_count + 1));
                trash[t_count].title = drafts[idx].title;
                trash[t_count].body = drafts[idx].body;
                t_count++;
                for (int i = idx; i < d_count - 1; i++) {
                    drafts[i] = drafts[i + 1];
                }
                d_count--;
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (t_count > 0) {
                t_count--;
                drafts = realloc(drafts, sizeof(Draft) * (d_count + 1));
                drafts[d_count] = trash[t_count];
                d_count++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int idx = find_title(title);
            if (idx != -1) {
                printf("%s\n", drafts[idx].body);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < d_count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }

    for (int i = 0; i < d_count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    for (int i = 0; i < t_count; i++) {
        free(trash[i].title);
        free(trash[i].body);
    }
    free(drafts);
    free(trash);

    return 0;
}