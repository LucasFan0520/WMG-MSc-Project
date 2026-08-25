// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARS 1000
#define MAX_LEN 1000

typedef struct {
    char key[100];
    char val[MAX_LEN];
} Var;

Var vars[MAX_VARS];
int count = 0;

int find_var(const char *key) {
    for (int i = 0; i < count; i++) if (strcmp(vars[i].key, key) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], key[100], val[MAX_LEN], text[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key);
            fgets(val, MAX_LEN, stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            int idx = find_var(key);
            if (idx == -1 && count < MAX_VARS) {
                strcpy(vars[count].key, key);
                strcpy(vars[count].val, val);
                count++;
            } else if (idx != -1) {
                strcpy(vars[idx].val, val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            scanf("%s", key);
            int idx = find_var(key);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) vars[i] = vars[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            char out[MAX_LEN * 2] = "";
            char *p = text;
            while (*p) {
                if (*p == '{') {
                    char *end = strchr(p, '}');
                    if (end) {
                        *end = 0;
                        int idx = find_var(p + 1);
                        if (idx != -1) strcat(out, vars[idx].val);
                        else {
                            strcat(out, "{");
                            strcat(out, p + 1);
                            strcat(out, "}");
                        }
                        p = end + 1;
                    } else {
                        char c[2] = {*p, 0};
                        strcat(out, c);
                        p++;
                    }
                } else {
                    char c[2] = {*p, 0};
                    strcat(out, c);
                    p++;
                }
            }
            printf("%s\n", out);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < count; i++) printf("%s=%s\n", vars[i].key, vars[i].val);
        }
    }
    return 0;
}