// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Var;

Var *vars = NULL;
int count = 0;

int find_var(const char *key) {
    for (int i = 0; i < count; i++)
        if (strcmp(vars[i].key, key) == 0) return i;
    return -1;
}

void render(const char *text) {
    for (int i = 0; text[i]; ) {
        if (text[i] == '{' && text[i+1] != '\0') {
            int j = i + 1;
            while (text[j] && text[j] != '}') j++;
            if (text[j] == '}') {
                char key[1000];
                strncpy(key, text + i + 1, j - i - 1);
                key[j - i - 1] = 0;
                int idx = find_var(key);
                if (idx != -1) printf("%s", vars[idx].val);
                else printf("%.*s", j - i + 1, text + i);
                i = j + 1;
            } else {
                putchar(text[i++]);
            }
        } else {
            putchar(text[i++]);
        }
    }
    putchar('\n');
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *val = line + 4 + strlen(arg1);
            if (*val == ' ') val++;
            int i = find_var(arg1);
            if (i != -1) {
                free(vars[i].val);
                vars[i].val = strdup(val);
            } else {
                vars = realloc(vars, sizeof(Var) * (count + 1));
                vars[count].key = strdup(arg1);
                vars[count].val = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_var(arg1);
            if (i != -1) {
                free(vars[i].key);
                free(vars[i].val);
                for (int j = i; j < count - 1; j++) vars[j] = vars[j + 1];
                count--;
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = line + 7;
            if (*text == ' ') text++;
            render(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < count; i++) printf("%s %s\n", vars[i].key, vars[i].val);
        }
    }
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].val);
    }
    free(vars);
    return 0;
}