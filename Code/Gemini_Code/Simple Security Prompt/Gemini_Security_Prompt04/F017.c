// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *key;
    char *value;
} Config;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (!fp) return 1;
    Config *cfgs = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(fp);
        if (!line) break;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                cfgs = realloc(cfgs, cap * sizeof(Config));
            }
            cfgs[count].key = strdup(line);
            cfgs[count].value = strdup(eq + 1);
            count++;
        }
        free(line);
    }
    fclose(fp);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                while (*val == ' ') val++;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfgs[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(cfgs[found].value);
                    cfgs[found].value = strdup(val);
                } else {
                    if (count >= cap) {
                        cap = cap == 0 ? 16 : cap * 2;
                        cfgs = realloc(cfgs, cap * sizeof(Config));
                    }
                    cfgs[count].key = strdup(key);
                    cfgs[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            while (*key == ' ') key++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfgs[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(cfgs[found].key);
                free(cfgs[found].value);
                for (int i = found; i < count - 1; i++) {
                    cfgs[i] = cfgs[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') key++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfgs[i].key, key) == 0) {
                    printf("%s\n", cfgs[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", cfgs[i].key, cfgs[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(cfgs[i].key);
        free(cfgs[i].value);
    }
    free(cfgs);
    return 0;
}
