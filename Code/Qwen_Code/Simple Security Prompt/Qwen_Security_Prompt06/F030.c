// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Rule {
    char *old;
    char *new_str;
} Rule;

Rule *rules = NULL;
int rule_cnt = 0;
int rule_cap = 0;

void add_rule(const char *old, const char *new_str) {
    if (rule_cnt == rule_cap) {
        rule_cap = rule_cap ? rule_cap * 2 : 4;
        rules = realloc(rules, rule_cap * sizeof(Rule));
    }
    rules[rule_cnt].old = strdup(old);
    rules[rule_cnt].new_str = strdup(new_str);
    rule_cnt++;
}

void apply_rule(const char *text) {
    for (int i = 0; i < rule_cnt; i++) {
        char *pos = strstr(text, rules[i].old);
        if (pos) {
            size_t old_len = strlen(rules[i].old);
            size_t new_len = strlen(rules[i].new_str);
            size_t res_len = strlen(text) - old_len + new_len + 1;
            char *res = malloc(res_len);
            size_t prefix_len = pos - text;
            strncpy(res, text, prefix_len);
            strcpy(res + prefix_len, rules[i].new_str);
            strcpy(res + prefix_len + new_len, pos + old_len);
            printf("%s\n", res);
            free(res);
            return;
        }
    }
    printf("%s\n", text);
}

void delete_rule(const char *old) {
    for (int i = 0; i < rule_cnt; i++) {
        if (strcmp(rules[i].old, old) == 0) {
            free(rules[i].old);
            free(rules[i].new_str);
            for (int j = i; j < rule_cnt - 1; j++) {
                rules[j] = rules[j+1];
            }
            rule_cnt--;
            return;
        }
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_cnt || j < 0 || j >= rule_cnt) return;
    Rule tmp = rules[i];
    rules[i] = rules[j];
    rules[j] = tmp;
}

void print_rules() {
    for (int i = 0; i < rule_cnt; i++) {
        printf("%s %s\n", rules[i].old, rules[i].new_str);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        int idx1 = 0, idx2 = 0;
        sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        
        if (strcmp(cmd, "ADD") == 0) add_rule(arg1, arg2);
        else if (strcmp(cmd, "APPLY") == 0) {
            char *text = get_rest(line, 2);
            if (text) apply_rule(text);
        }
        else if (strcmp(cmd, "DELETE") == 0) delete_rule(arg1);
        else if (strcmp(cmd, "SWAP") == 0) {
            sscanf(line, "%*s %d %d", &idx1, &idx2);
            swap_rules(idx1, idx2);
        }
        else if (strcmp(cmd, "RULES") == 0) print_rules();
        
        free(line);
    }
    for (int i = 0; i < rule_cnt; i++) {
        free(rules[i].old);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}