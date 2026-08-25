// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct V {
    char *key;
    char *val;
    struct V *next;
} V;

V *head = NULL;

int main() {
    char cmd[20];
    char key[100];
    char val[1000];
    char text[2000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "SET")) {
            scanf("%s", key);
            fgets(val, 1000, stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') {
                memmove(val, val + 1, strlen(val));
            }
            V *p = head;
            while (p && strcmp(p->key, key)) {
                p = p->next;
            }
            if (p) {
                free(p->val);
                p->val = strdup(val);
            } else {
                V *nv = malloc(sizeof(V));
                nv->key = strdup(key);
                nv->val = strdup(val);
                nv->next = head;
                head = nv;
            }
        } else if (!strcmp(cmd, "UNSET")) {
            scanf("%s", key);
            V *p = head;
            V *prev = NULL;
            while (p) {
                if (!strcmp(p->key, key)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    free(p->key);
                    free(p->val);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "RENDER")) {
            fgets(text, 2000, stdin);
            text[strcspn(text, "\n")] = 0;
            char out[4000];
            int oi = 0;
            for (int i = 0; text[i]; ) {
                if (text[i] == '{') {
                    int j = i + 1;
                    while (text[j] && text[j] != '}') {
                        j++;
                    }
                    if (text[j] == '}') {
                        char k[100];
                        strncpy(k, text + i + 1, j - i - 1);
                        k[j - i - 1] = 0;
                        V *p = head;
                        while (p && strcmp(p->key, k)) {
                            p = p->next;
                        }
                        if (p) {
                            strcpy(out + oi, p->val);
                            oi += strlen(p->val);
                            i = j + 1;
                            continue;
                        }
                    }
                }
                out[oi++] = text[i++];
            }
            out[oi] = 0;
            printf("%s\n", out);
        } else if (!strcmp(cmd, "VARS")) {
            for (V *p = head; p; p = p->next) {
                printf("%s=%s\n", p->key, p->val);
            }
        }
    }
    return 0;
}