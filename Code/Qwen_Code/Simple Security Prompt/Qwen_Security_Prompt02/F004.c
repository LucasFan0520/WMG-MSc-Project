// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Variable;

Variable *vars = NULL;
int count = 0;

int find_key(const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(vars[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void render(const char *text) {
    char out[100000] = "";
    int i = 0;
    while (text[i]) {
        if (text[i] == '{') {
            int j = i + 1;
            while (text[j] && text[j] != '}') {
                j++;
            }
            if (text[j] == '}') {
                char key[1000] = "";
                strncpy(key, text + i + 1, j - i - 1);
                int idx = find_key(key);
                if (idx != -1) {
                    strcat(out, vars[idx].val);
                } else {
                    strncat(out, text + i, j - i + 1);
                }
                i = j + 1;
            } else {
                out[strlen(out)] = text[i++];
            }
        } else {
            out[strlen(out)] = text[i++];
        }
    }
    printf("%s\n", out);
}

int main() {
    char cmd[20];
    char key[1000];
    char val[10000];
    char text[100000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key);
            fgets(val, sizeof(val), stdin);
            if (val[0] == ' ') {
                memmove(val, val + 1, strlen(val));
            }
            val[strcspn(val, "\n")] = 0;
            int idx = find_key(key);
            if (idx != -1) {
                free(vars[idx].val);
                vars[idx].val = strdup(val);
            } else {
                vars = realloc(vars, sizeof(Variable) * (count + 1));
                vars[count].key = strdup(key);
                vars[count].val = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                free(vars[idx].key);
                free(vars[idx].val);
                for (int i = idx; i < count - 1; i++) {
                    vars[i] = vars[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            fgets(text, sizeof(text), stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            text[strcspn(text, "\n")] = 0;
            render(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", vars[i].key, vars[i].val);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].val);
    }
    free(vars);

    return 0;
}