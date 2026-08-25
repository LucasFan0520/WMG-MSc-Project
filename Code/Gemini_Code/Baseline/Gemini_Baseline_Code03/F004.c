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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            int key_len = 0;
            if (sscanf(ptr, "%255s%n", key, &key_len) > 0) {
                char *val = ptr + key_len;
                while (*val == ' ') val++;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(vars[found].value);
                    vars[found].value = strdup(val);
                } else {
                    vars = realloc(vars, (count + 1) * sizeof(Variable));
                    vars[count].key = strdup(key);
                    vars[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(ptr, "%255s", key) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(vars[found].key);
                    free(vars[found].value);
                    for (int i = found; i < count - 1; i++) {
                        vars[i] = vars[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        vars = realloc(vars, count * sizeof(Variable));
                    } else {
                        free(vars);
                        vars = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *t = ptr;
            while (*t) {
                if (*t == '{') {
                    char *end = strchr(t, '}');
                    if (end) {
                        int k_len = end - t - 1;
                        char *k_buf = malloc(k_len + 1);
                        strncpy(k_buf, t + 1, k_len);
                        k_buf[k_len] = '\0';
                        int found = -1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, k_buf) == 0) {
                                found = i;
                                break;
                            }
                        }
                        if (found != -1) {
                            printf("%s", vars[found].value);
                            t = end + 1;
                        } else {
                            putchar('{');
                            t++;
                        }
                        free(k_buf);
                    } else {
                        putchar('{');
                        t++;
                    }
                } else {
                    putchar(*t);
                    t++;
                }
            }
            putchar('\n');
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
