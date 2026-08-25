// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_OLD 64
#define MAX_NEW 4096

typedef struct Rule {
    char old[MAX_OLD];
    char new[MAX_NEW];
    struct Rule *next;
} Rule;

Rule *head = NULL;
int rule_count = 0;

Rule *get_rule(int index) {
    if (index < 0 || index >= rule_count) return NULL;
    Rule *r = head;
    for (int i = 0; i < index; i++) r = r->next;
    return r;
}

void free_all() {
    Rule *r = head;
    while (r) {
        Rule *tmp = r;
        r = r->next;
        free(tmp);
    }
    head = NULL;
    rule_count = 0;
}

void handle_add(char *line) {
    char old[MAX_OLD], new[MAX_NEW];
    int n = sscanf(line, "ADD %63s %4095[^\n]", old, new);
    if (n < 2) return;
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
    strcpy(r->old, old);
    strcpy(r->new, new);
    r->next = head;
    head = r;
    rule_count++;
}

void handle_apply(char *line) {
    char text[MAX_LINE];
    if (sscanf(line, "APPLY %4095[^\n]", text) != 1) return;
    Rule *r = head;
    int found = 0;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            // Replace first occurrence
            char result[MAX_LINE];
            int prefix_len = pos - text;
            int old_len = strlen(r->old);
            int new_len = strlen(r->new);
            if (prefix_len + new_len + strlen(pos + old_len) + 1 > MAX_LINE) return;
            snprintf(result, sizeof(result), "%.*s%s%s", prefix_len, text, r->new, pos + old_len);
            printf("%s\n", result);
            found = 1;
            break;
        }
        r = r->next;
    }
    if (!found) printf("%s\n", text);
}

void handle_delete(char *line) {
    char old[MAX_OLD];
    if (sscanf(line, "DELETE %63s", old) != 1) return;
    Rule *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r);
            rule_count--;
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_swap(char *line) {
    int i, j;
    if (sscanf(line, "SWAP %d %d", &i, &j) != 2) return;
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count) return;
    if (i == j) return;
    Rule *ri = get_rule(i);
    Rule *rj = get_rule(j);
    // swap data
    char tmp_old[MAX_OLD], tmp_new[MAX_NEW];
    strcpy(tmp_old, ri->old);
    strcpy(tmp_new, ri->new);
    strcpy(ri->old, rj->old);
    strcpy(ri->new, rj->new);
    strcpy(rj->old, tmp_old);
    strcpy(rj->new, tmp_new);
}

void handle_rules() {
    Rule *r = head;
    int idx = 0;
    while (r) {
        printf("%d %s %s\n", idx, r->old, r->new);
        r = r->next;
        idx++;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "APPLY ", 6) == 0) handle_apply(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "SWAP ", 5) == 0) handle_swap(line);
        else if (strcmp(line, "RULES") == 0) handle_rules();
    }
    free_all();
    return 0;
}