// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var {
    char *key, *val;
    struct Var *next;
} Var;

Var *head = NULL;

void read_rest(char *buf) {
    if (!fgets(buf, 4096, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

int main() {
    char cmd[20], key[256], text[4096];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key); read_rest(text);
            Var *v = head;
            while (v && strcmp(v->key, key) != 0) v = v->next;
            if (v) { free(v->val); v->val = strdup(text); }
            else {
                Var *nv = malloc(sizeof(Var));
                nv->key = strdup(key); nv->val = strdup(text); nv->next = head; head = nv;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            scanf("%s", key);
            Var *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->key, key) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->key); free(curr->val); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            read_rest(text);
            char out[8192] = "";
            int oi = 0;
            for (int i = 0; text[i]; ) {
                if (text[i] == '{') {
                    int j = i + 1;
                    while (text[j] && text[j] != '}') j++;
                    if (text[j] == '}') {
                        char k[256];
                        strncpy(k, text + i + 1, j - i - 1);
                        k[j - i - 1] = '\0';
                        Var *v = head;
                        while (v && strcmp(v->key, k) != 0) v = v->next;
                        if (v) {
                            strcpy(out + oi, v->val);
                            oi += strlen(v->val);
                            i = j + 1;
                            continue;
                        }
                    }
                }
                out[oi++] = text[i++];
            }
            out[oi] = '\0';
            printf("%s\n", out);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (Var *v = head; v; v = v->next) printf("%s=%s\n", v->key, v->val);
        }
    }
    return 0;
}