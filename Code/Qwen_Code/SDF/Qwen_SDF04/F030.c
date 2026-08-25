// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* old_str; char* new_str; } Rule;

Rule* rules = NULL;
size_t r_count = 0, r_cap = 0;

void cleanup() {
    for (size_t i = 0; i < r_count; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* old_s = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* new_s = ptr;

            if (r_count == r_cap) {
                size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                Rule* temp = realloc(rules, new_cap * sizeof(Rule));
                if (!temp) { cleanup(); free(line); return 1; }
                rules = temp; r_cap = new_cap;
            }
            rules[r_count].old_str = strdup(old_s);
            rules[r_count].new_str = strdup(new_s);
            if (!rules[r_count].old_str || !rules[r_count].new_str) { cleanup(); free(line); return 1; }
            r_count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* text = ptr;
            int applied = 0;
            for (size_t i = 0; i < r_count; i++) {
                char* pos = strstr(text, rules[i].old_str);
                if (pos) {
                    size_t pre_len = pos - text;
                    size_t old_len = strlen(rules[i].old_str);
                    size_t new_len = strlen(rules[i].new_str);
                    size_t post_len = strlen(pos + old_len);
                    size_t total = pre_len + new_len + post_len;
                    
                    char* res = malloc(total + 1);
                    if (!res) { cleanup(); free(line); return 1; }
                    memcpy(res, text, pre_len);
                    memcpy(res + pre_len, rules[i].new_str, new_len);
                    memcpy(res + pre_len + new_len, pos + old_len, post_len + 1);
                    printf("%s\n", res);
                    free(res);
                    applied = 1;
                    break;
                }
            }
            if (!applied) printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(rules[i].old_str, ptr) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (size_t j = i; j < r_count - 1; j++) rules[j] = rules[j + 1];
                    r_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* i_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* j_str = ptr;

            long i = strtol(i_str, NULL, 10);
            long j = strtol(j_str, NULL, 10);
            if (i >= 0 && i < (long)r_count && j >= 0 && j < (long)r_count) {
                Rule temp = rules[i];
                rules[i] = rules[j];
                rules[j] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%s %s\n", rules[i].old_str, rules[i].new_str);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}