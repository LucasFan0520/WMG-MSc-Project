// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Rule {
    char* old;
    char* new_str;
    struct Rule* next;
} Rule;

Rule* head = NULL;

int count_rules() {
    int c = 0;
    for (Rule* r = head; r; r = r->next) c++;
    return c;
}

Rule* get_rule(int idx) {
    Rule* r = head;
    for (int i = 0; i < idx && r; i++) r = r->next;
    return r;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char* old = get_word(&p);
            if (old) {
                Rule* n = malloc(sizeof(Rule));
                n->old = strdup(old);
                n->new_str = strdup(p);
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char* text = p;
            char out[MAX_LINE * 2] = {0};
            int applied = 0;
            for (Rule* r = head; r; r = r->next) {
                char* pos = strstr(text, r->old);
                if (pos) {
                    size_t prefix_len = pos - text;
                    strncpy(out, text, prefix_len);
                    out[prefix_len] = 0;
                    strcat(out, r->new_str);
                    strcat(out, pos + strlen(r->old));
                    applied = 1;
                    break;
                }
            }
            if (applied) printf("%s\n", out);
            else printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* old = get_word(&p);
            Rule* prev = NULL;
            for (Rule* r = head; r; prev = r, r = r->next) {
                if (strcmp(r->old, old) == 0) {
                    if (prev) prev->next = r->next;
                    else head = r->next;
                    free(r->old);
                    free(r->new_str);
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char* i_str = get_word(&p);
            char* j_str = get_word(&p);
            int i = atoi(i_str);
            int j = atoi(j_str);
            int total = count_rules();
            if (i < 0 || i >= total || j < 0 || j >= total || i == j) continue;
            Rule* ri = get_rule(i);
            Rule* rj = get_rule(j);
            char* temp_old = ri->old;
            char* temp_new = ri->new_str;
            ri->old = rj->old;
            ri->new_str = rj->new_str;
            rj->old = temp_old;
            rj->new_str = temp_new;
        } else if (strcmp(cmd, "RULES") == 0) {
            for (Rule* r = head; r; r = r->next) {
                printf("%s %s\n", r->old, r->new_str);
            }
        }
    }
    return 0;
}