// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old_str, *new_str;
    struct Rule *next;
} Rule;

Rule *head = NULL;
int count = 0;

Rule *get_at(int idx) {
    Rule *curr = head;
    for (int i = 0; i < idx; i++) curr = curr->next;
    return curr;
}

int main() {
    char cmd[20], old_str[256], new_str[256], text[1024];
    int i, j;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", old_str, new_str);
            Rule *r = malloc(sizeof(Rule));
            r->old_str = strdup(old_str); r->new_str = strdup(new_str); r->next = NULL;
            if (!head) head = r; else { Rule *c = head; while (c->next) c = c->next; c->next = r; }
            count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            scanf("%s", text);
            int found = 0;
            for (Rule *r = head; r; r = r->next) {
                char *pos = strstr(text, r->old_str);
                if (pos) {
                    char res[2048];
                    int len = pos - text;
                    strncpy(res, text, len);
                    res[len] = '\0';
                    strcat(res, r->new_str);
                    strcat(res, pos + strlen(r->old_str));
                    printf("%s\n", res);
                    found = 1;
                    break;
                }
            }
            if (!found) printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", old_str);
            Rule *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->old_str, old_str) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->old_str); free(curr->new_str); free(curr);
                    count--;
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            scanf("%d %d", &i, &j);
            if (i < 0 || i >= count || j < 0 || j >= count || i == j) continue;
            Rule *ri = get_at(i), *rj = get_at(j);
            char *tmp_o = ri->old_str; ri->old_str = rj->old_str; rj->old_str = tmp_o;
            char *tmp_n = ri->new_str; ri->new_str = rj->new_str; rj->new_str = tmp_n;
        } else if (strcmp(cmd, "RULES") == 0) {
            for (Rule *r = head; r; r = r->next) printf("%s %s\n", r->old_str, r->new_str);
        }
    }
    return 0;
}