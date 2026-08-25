// F017.c
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

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (f) {
        char line[MAX_LEN];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = 0;
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = 0;
                char *val = eq + 1;
                if (val[0] == ' ') val++;
                int idx = find_var(line);
                if (idx == -1 && count < MAX_VARS) {
                    strcpy(vars[count].key, line);
                    strcpy(vars[count].val, val);
                    count++;
                } else if (idx != -1) {
                    strcpy(vars[idx].val, val);
                }
            }
        }
        fclose(f);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char cmd[20], key[100], val[MAX_LEN];
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
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s", key);
            int idx = find_var(key);
            if (idx != -1) printf("%s\n", vars[idx].val);
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < count; i++) printf("%s=%s\n", vars[i].key, vars[i].val);
        }
    }
    return 0;
}