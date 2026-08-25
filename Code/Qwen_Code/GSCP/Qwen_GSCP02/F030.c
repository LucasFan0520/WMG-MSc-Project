/* F030.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

static Rule *rules = NULL;
static int rcount = 0;
static int rcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char old_s[256] = {0}, new_s[256] = {0};
            if (sscanf(line + offset, "%255s %255s", old_s, new_s) != 2) continue;
            if (rcount >= rcap) {
                rcap = rcap == 0 ? 8 : rcap * 2;
                rules = realloc(rules, sizeof(Rule) * rcap);
            }
            rules[rcount].old_str = dup_str(old_s);
            rules[rcount].new_str = dup_str(new_s);
            rcount++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            const char *text = line + offset;
            while (*text == ' ') text++;
            for (int i = 0; i < rcount; i++) {
                char *pos = strstr(text, rules[i].old_str);
                if (pos) {
                    size_t pre = (size_t)(pos - text);
                    size_t old_len = strlen(rules[i].old_str);
                    size_t new_len = strlen(rules[i].new_str);
                    size_t res_len = pre + new_len + strlen(pos + old_len);
                    char *res = malloc(res_len + 1);
                    if (!res) break;
                    memcpy(res, text, pre);
                    memcpy(res + pre, rules[i].new_str, new_len);
                    memcpy(res + pre + new_len, pos + old_len, strlen(pos + old_len) + 1);
                    printf("%s\n", res);
                    free(res);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old_s[256] = {0};
            if (sscanf(line + offset, "%255s", old_s) != 1) continue;
            for (int i = 0; i < rcount; i++) {
                if (strcmp(rules[i].old_str, old_s) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (int j = i; j < rcount - 1; j++) rules[j] = rules[j + 1];
                    rcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i = 0, j = 0;
            if (sscanf(line + offset, "%d %d", &i, &j) != 2) continue;
            if (i < 0 || i >= rcount || j < 0 || j >= rcount) continue;
            Rule tmp = rules[i];
            rules[i] = rules[j];
            rules[j] = tmp;
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s %s\n", rules[i].old_str, rules[i].new_str);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}