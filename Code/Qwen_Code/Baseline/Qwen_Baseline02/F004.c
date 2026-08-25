// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } Var;
Var v[1000]; int nv = 0;

int find(char *key) {
    for (int i = 0; i < nv; i++) if (!strcmp(v[i].key, key)) return i;
    return -1;
}

int main() {
    char cmd[20], key[100], val[10000], text[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "SET")) {
            scanf("%s", key); fgets(val, sizeof(val), stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            int idx = find(key);
            if (idx != -1) { free(v[idx].val); v[idx].val = strdup(val); }
            else { v[nv].key = strdup(key); v[nv++].val = strdup(val); }
        } else if (!strcmp(cmd, "UNSET")) {
            scanf("%s", key); int idx = find(key);
            if (idx != -1) {
                free(v[idx].key); free(v[idx].val);
                for (int i = idx; i < nv - 1; i++) v[i] = v[i + 1];
                nv--;
            }
        } else if (!strcmp(cmd, "RENDER")) {
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            char out[10000] = "";
            for (int i = 0; text[i]; ) {
                if (text[i] == '{') {
                    int j = i + 1;
                    while (text[j] && text[j] != '}') j++;
                    if (text[j] == '}') {
                        char k[100] = "";
                        strncpy(k, text + i + 1, j - i - 1);
                        int idx = find(k);
                        if (idx != -1) strcat(out, v[idx].val);
                        else { strncat(out, text + i, j - i + 1); }
                        i = j + 1;
                    } else { out[strlen(out)] = text[i++]; }
                } else { out[strlen(out)] = text[i++]; }
            }
            printf("%s\n", out);
        } else if (!strcmp(cmd, "VARS")) {
            for (int i = 0; i < nv; i++) printf("%s %s\n", v[i].key, v[i].val);
        }
    }
    for (int i = 0; i < nv; i++) { free(v[i].key); free(v[i].val); }
    return 0;
}