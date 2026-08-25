// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigPair;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    ConfigPair *pairs = NULL;
    int count = 0;
    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    if (fp) {
        while ((read = getline(&line, &cap, fp)) != -1) {
            line[strcspn(line, "\r\n")] = 0;
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                ConfigPair *tmp = realloc(pairs, (count + 1) * sizeof(ConfigPair));
                if (tmp) {
                    pairs = tmp;
                    pairs[count].key = strdup(line);
                    pairs[count].value = strdup(eq + 1);
                    count++;
                }
            }
        }
        fclose(fp);
    }
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
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(pairs[i].key, key) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                char *nv = strdup(val);
                if (nv) {
                    free(pairs[idx].value);
                    pairs[idx].value = nv;
                }
            } else {
                ConfigPair *tmp = realloc(pairs, (count + 1) * sizeof(ConfigPair));
                if (tmp) {
                    pairs = tmp;
                    pairs[count].key = strdup(key);
                    pairs[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            while (*key == ' ') {
                key++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(pairs[i].key, key) == 0) {
                    free(pairs[i].key);
                    free(pairs[i].value);
                    for (int j = i; j < count - 1; j++) {
                        pairs[j] = pairs[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') {
                key++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(pairs[i].key, key) == 0) {
                    printf("%s\n", pairs[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", pairs[i].key, pairs[i].value);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(pairs[i].key);
        free(pairs[i].value);
    }
    free(pairs);
    return 0;
}
