// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line_from(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *key;
    char *value;
} ConfigPair;

int main(int argc, char **argv) {
    ConfigPair *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line_from(fp);
                if (!line) break;
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = line;
                    char *val = eq + 1;
                    size_t vlen = strlen(val);
                    while (vlen > 0 && (val[vlen-1] == '\n' || val[vlen-1] == '\n')) {
                        val[vlen-1] = '\0';
                        vlen--;
                    }
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        ConfigPair *nitems = realloc(items, cap * sizeof(ConfigPair));
                        if (nitems) items = nitems;
                    }
                    items[count].key = mystrdup(key);
                    items[count].value = mystrdup(val);
                    count++;
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line_from(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *val = p;
            size_t vlen = strlen(val);
            while (vlen > 0 && (val[vlen-1] == '\n' || val[vlen-1] == '\n')) {
                val[vlen-1] = '\0';
                vlen--;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(items[found].value);
                items[found].value = mystrdup(val);
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    ConfigPair *nitems = realloc(items, cap * sizeof(ConfigPair));
                    if (nitems) items = nitems;
                }
                items[count].key = mystrdup(key);
                items[count].value = mystrdup(val);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = p;
            size_t klen = strlen(key);
            while (klen > 0 && (key[klen-1] == '\n' || key[klen-1] == '\n')) {
                key[klen-1] = '\0';
                klen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    free(items[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = p;
            size_t klen = strlen(key);
            while (klen > 0 && (key[klen-1] == '\n' || key[klen-1] == '\n')) {
                key[klen-1] = '\0';
                klen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    printf("%s\n", items[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", items[i].key, items[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].key);
        free(items[i].value);
    }
    free(items);
    return 0;
}
