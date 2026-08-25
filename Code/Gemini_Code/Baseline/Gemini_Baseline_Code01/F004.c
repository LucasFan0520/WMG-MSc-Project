// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

int main() {
    Variable *vars = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            int key_bytes = 0;
            if (sscanf(args, "%255s%n", key, &key_bytes) > 0) {
                char *val = args + key_bytes;
                while (*val == ' ') val++;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        free(vars[i].value);
                        vars[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    vars = realloc(vars, (count + 1) * sizeof(Variable));
                    vars[count].key = strdup(key);
                    vars[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(args, "%255s", key) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        free(vars[i].key);
                        free(vars[i].value);
                        for (int j = i; j < count - 1; j++) {
                            vars[j] = vars[j + 1];
                        }
                        count--;
                        vars = realloc(vars, count * sizeof(Variable));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *p = args;
            while (*p != '\0') {
                if (*p == '{') {
                    char *end = strchr(p, '}');
                    if (end) {
                        int key_len = end - p - 1;
                        char *key_buf = malloc(key_len + 1);
                        strncpy(key_buf, p + 1, key_len);
                        key_buf[key_len] = '\0';
                        char *val = NULL;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, key_buf) == 0) {
                                val = vars[i].value;
                                break;
                            }
                        }
                        if (val) {
                            printf("%s", val);
                            p = end + 1;
                        } else {
                            printf("{%s}", key_buf);
                            p = end + 1;
                        }
                        free(key_buf);
                    } else {
                        printf("%c", *p);
                        p++;
                    }
                } else {
                    printf("%c", *p);
                    p++;
                }
            }
            printf("\n");
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].value);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
