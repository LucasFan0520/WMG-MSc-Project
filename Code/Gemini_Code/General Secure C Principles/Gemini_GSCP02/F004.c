// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Var;

int main(void) {
    Var *vars = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') {
                key++;
            }
            char *sp = strchr(key, ' ');
            char *val = "";
            if (sp) {
                *sp = '\0';
                val = sp + 1;
                while (*val == ' ') {
                    val++;
                }
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *nv = strdup(val);
                if (nv) {
                    free(vars[found].value);
                    vars[found].value = nv;
                }
            } else {
                Var *tmp = realloc(vars, (count + 1) * sizeof(Var));
                if (tmp) {
                    vars = tmp;
                    vars[count].key = strdup(key);
                    vars[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            while (*key == ' ') {
                key++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].key);
                    free(vars[i].value);
                    for (int j = i; j < count - 1; j++) {
                        vars[j] = vars[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            while (*text == ' ') {
                text++;
            }
            int len = strlen(text);
            for (int i = 0; i < len; i++) {
                if (text[i] == '{') {
                    int j = i + 1;
                    while (j < len && text[j] != '}') {
                        j++;
                    }
                    if (j < len && text[j] == '}') {
                        int klen = j - i - 1;
                        char *kbuf = malloc(klen + 1);
                        if (kbuf) {
                            strncpy(kbuf, text + i + 1, klen);
                            kbuf[klen] = '\0';
                            char *found_val = NULL;
                            for (int v = 0; v < count; v++) {
                                if (strcmp(vars[v].key, kbuf) == 0) {
                                    found_val = vars[v].value;
                                    break;
                               }
                            }
                            if (found_val) {
                                printf("%s", found_val);
                            } else {
                                for (int m = i; m <= j; m++) {
                                    putchar(text[m]);
                                }
                            }
                            free(kbuf);
                        }
                        i = j;
                    } else {
                        putchar(text[i]);
                    }
                } else {
                    putchar(text[i]);
                }
            }
            putchar('\n');
        } else if (strcmp(line, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", vars[i].key, vars[i].value);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
