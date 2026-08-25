// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var {
    char *key, *val;
    struct Var *next;
} Var;

Var *head = NULL;

void free_list() {
    while (head) {
        Var *t = head; head = head->next;
        free(t->key); free(t->val); free(t);
    }
}

Var *find_key(const char *key) {
    for (Var *v = head; v; v = v->next)
        if (strcmp(v->key, key) == 0) return v;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], key[256], val[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) < 1) continue;
            Var *v = find_key(key);
            if (v) { free(v->val); v->val = strdup(val); }
            else {
                v = malloc(sizeof(Var));
                v->key = strdup(key); v->val = strdup(val); v->next = head; head = v;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) != 1) continue;
            Var *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->key, key) == 0) {
                    Var *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->key); free(tmp->val); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", val) != 1) continue;
            char out[8192] = "";
            int oi = 0;
            for (int i = 0; val[i]; ) {
                if (val[i] == '{') {
                    int j = i + 1;
                    while (val[j] && val[j] != '}') j++;
                    if (val[j] == '}') {
                        char k[256];
                        strncpy(k, val + i + 1, j - i - 1);
                        k[j - i - 1] = 0;
                        Var *v = find_key(k);
                        if (v) {
                            int len = strlen(v->val);
                            strcpy(out + oi, v->val);
                            oi += len;
                        } else {
                            strncpy(out + oi, val + i, j - i + 1);
                            oi += j - i + 1;
                        }
                        i = j + 1;
                    } else {
                        out[oi++] = val[i++];
                    }
                } else {
                    out[oi++] = val[i++];
                }
            }
            out[oi] = 0;
            printf("%s\n", out);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (Var *v = head; v; v = v->next) printf("%s=%s\n", v->key, v->val);
        }
    }
    free_list();
    return 0;
}