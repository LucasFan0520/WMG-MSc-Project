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
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *key;
    char *value;
} ConfigPair;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    ConfigPair *cfg = NULL;
    size_t count = 0;
    size_t cap = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        while (1) {
            char *fline = read_line(fp);
            if (!fline) break;
            char *eq = strchr(fline, '=');
            if (eq) {
                *eq = '\0';
                char *val = eq + 1;
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    ConfigPair *nb = realloc(cfg, cap * sizeof(ConfigPair));
                    if (!nb) {
                        free(fline);
                        break;
                    }
                    cfg = nb;
                }
                cfg[count].key = safe_dup(fline);
                cfg[count].value = safe_dup(val);
                count++;
            }
            free(fline);
        }
        fclose(fp);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *key = p;
            while (*p && *p != ' ') p++;
            char *value = "";
            if (*p == ' ') {
                *p = '\0';
                value = p + 1;
            }
            size_t found = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != (size_t)-1) {
                free(cfg[found].value);
                cfg[found].value = safe_dup(value);
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    ConfigPair *nb = realloc(cfg, cap * sizeof(ConfigPair));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    cfg = nb;
                }
                cfg[count].key = safe_dup(key);
                cfg[count].value = safe_dup(value);
                count++;
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, key) == 0) {
                    free(cfg[i].key);
                    free(cfg[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        cfg[j] = cfg[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, key) == 0) {
                    printf("%s\n", cfg[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", cfg[i].key, cfg[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(cfg[i].key);
        free(cfg[i].value);
    }
    free(cfg);
    return 0;
}
