// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char *value;
} ConfigPair;

int main(int argc, char **argv) {
    ConfigPair *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *fline = read_line(fp);
                if (!fline) break;
                char *eq = strchr(fline, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = fline;
                    char *val = eq + 1;
                    size_t vl = strlen(val);
                    while (vl > 0 && (val[vl - 1] == '\n' || val[vl - 1] == '\r')) {
                        val[vl - 1] = '\0';
                        vl--;
                    }
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        ConfigPair *narr = realloc(arr, ncap * sizeof(ConfigPair));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].key = strdup(key);
                        arr[count].value = strdup(val);
                        count++;
                    }
                }
                free(fline);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "WRITE") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s=%s\n", arr[i].key, arr[i].value);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "SET") == 0) {
            char *key = args;
            char *p2 = strchr(key, ' ');
            if (p2) {
                *p2 = '\0';
                char *val = p2 + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    char *nval = strdup(val);
                    if (nval) {
                        free(arr[found].value);
                        arr[found].value = nval;
                    }
                } else {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        ConfigPair *narr = realloc(arr, ncap * sizeof(ConfigPair));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].key = strdup(key);
                        arr[count].value = strdup(val);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].key);
                    free(arr[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    printf("%s\n", arr[i].value);
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].key);
        free(arr[i].value);
    }
    free(arr);
    return 0;
}
