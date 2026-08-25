// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Var;

Var *vars = NULL;
int var_count = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "SET") == 0) {
            char key[100], val[1000];
            scanf("%s", key);
            scanf(" %[^\n]", val);
            int idx = -1;
            for (int i = 0; i < var_count; i++) {
                if (strcmp(vars[i].key, key) == 0) { idx = i; break; }
            }
            if (idx != -1) {
                free(vars[idx].val);
                vars[idx].val = strdup(val);
            } else {
                vars = realloc(vars, sizeof(Var) * (var_count + 1));
                vars[var_count].key = strdup(key);
                vars[var_count].val = strdup(val);
                var_count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[100];
            scanf("%s", key);
            for (int i = 0; i < var_count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].key);
                    free(vars[i].val);
                    for (int j = i; j < var_count - 1; j++) vars[j] = vars[j+1];
                    var_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char text[2000];
            scanf(" %[^\n]", text);
            char out[4000] = "";
            char *p = text;
            while (*p) {
                if (*p == '{') {
                    char *end = strchr(p, '}');
                    if (end) {
                        char key[100];
                        strncpy(key, p + 1, end - p - 1);
                        key[end - p - 1] = '\0';
                        int found = 0;
                        for (int i = 0; i < var_count; i++) {
                            if (strcmp(vars[i].key, key) == 0) {
                                strcat(out, vars[i].val);
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            strncat(out, p, end - p + 1);
                        }
                        p = end + 1;
                    } else {
                        strcat(out, "{");
                        p++;
                    }
                } else {
                    char c[2] = {*p, '\0'};
                    strcat(out, c);
                    p++;
                }
            }
            printf("%s\n", out);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < var_count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].val);
            }
        }
    }
    for (int i = 0; i < var_count; i++) {
        free(vars[i].key);
        free(vars[i].val);
    }
    free(vars);
    return 0;
}