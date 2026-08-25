// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
    struct Rule *next;
} Rule;

Rule *head = NULL;
int count = 0;

Rule *get_rule_at(int index) {
    if (index < 0 || index >= count) return NULL;
    Rule *r = head;
    for (int i = 0; i < index; i++) r = r->next;
    return r;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = head;
    head = r;
    count++;
}

void apply_rule(const char *text) {
    Rule *r = head;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            // replace first occurrence
            size_t len = strlen(text) + strlen(r->new) - strlen(r->old) + 1;
            char *result = malloc(len);
            if (!result) return;
            strncpy(result, text, pos - text);
            result[pos - text] = '\0';
            strcat(result, r->new);
            strcat(result, pos + strlen(r->old));
            for (char *p = result; *p; p++) {
                putchar(*p == ' ' ? '_' : *p);
            }
            putchar('\n');
            free(result);
            return;
        }
        r = r->next;
    }
    // no match: print as is with underscores
    for (char *p = text; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void delete_rule(const char *old) {
    Rule *prev = NULL;
    Rule *r = head;
    while (r) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r->old);
            free(r->new);
            free(r);
            count--;
            return;
        }
        prev = r;
        r = r->next;
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= count || j < 0 || j >= count || i == j) return;
    Rule *ri = get_rule_at(i);
    Rule *rj = get_rule_at(j);
    if (!ri || !rj) return;
    // swap old and new
    char *tmp_old = ri->old;
    char *tmp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = tmp_old;
    rj->new = tmp_new;
}

void print_rules(void) {
    Rule *r = head;
    while (r) {
        printf("%s -> %s\n", r->old, r->new);
        r = r->next;
    }
}

void free_all(void) {
    while (head) {
        Rule *next = head->next;
        free(head->old);
        free(head->new);
        free(head);
        head = next;
    }
    count = 0;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char old[256], new[256];
            if (sscanf(line, "%*s %255s %255s", old, new) == 2) {
                add_rule(old, new);
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char text[1024];
            if (sscanf(line, "%*s %1023[^\n]", text) == 1) {
                apply_rule(text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old[256];
            if (sscanf(line, "%*s %255s", old) == 1) {
                delete_rule(old);
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            if (sscanf(line, "%*s %d %d", &i, &j) == 2) {
                swap_rules(i, j);
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }

    free(line);
    free_all();
    return 0;
}