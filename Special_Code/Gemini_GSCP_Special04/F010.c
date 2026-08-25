/* F010.c */
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
        len--;
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
    char **values;
    size_t v_count;
    size_t v_cap;
} KeyIndex;

int main(void) {
    KeyIndex *index = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *val = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    KeyIndex *ni = realloc(index, cap * sizeof(KeyIndex));
                    if (ni) index = ni;
                }
                char *nk = mystrdup(key);
                if (nk) {
                    index[count].key = nk;
                    index[count].values = NULL;
                    index[count].v_count = 0;
                    index[count].v_cap = 0;
                    found = (int)count;
                    count++;
                }
            }
            if (found != -1) {
                KeyIndex *ki = &index[found];
                if (ki->v_count >= ki->v_cap) {
                    ki->v_cap = ki->v_cap == 0 ? 4 : ki->v_cap * 2;
                    char **nv = realloc(ki->values, ki->v_cap * sizeof(char *));
                    if (nv) ki->values = nv;
                }
                char *nv_val = mystrdup(val);
                if (nv_val) {
                    ki->values[ki->v_count] = nv_val;
                    ki->v_count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *val = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                KeyIndex *ki = &index[found];
                int v_found = -1;
                for (size_t j = 0; j < ki->v_count; j++) {
                    if (strcmp(ki->values[j], val) == 0) {
                        v_found = (int)j;
                        break;
                    }
                }
                if (v_found != -1) {
                    free(ki->values[v_found]);
                    for (size_t j = (size_t)v_found; j < ki->v_count - 1; j++) {
                        ki->values[j] = ki->values[j + 1];
                    }
                    ki->v_count--;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(index[found].key);
                for (size_t j = 0; j < index[found].v_count; j++) {
                    free(index[found].values[j]);
                }
                free(index[found].values);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    index[i] = index[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                for (size_t j = 0; j < index[found].v_count; j++) {
                    printf("%s", index[found].values[j]);
                    if (j + 1 < index[found].v_count) putchar(' ');
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", index[i].key);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(index[i].key);
        for (size_t j = 0; j < index[i].v_count; j++) {
            free(index[i].values[j]);
        }
        free(index[i].values);
    }
    free(index);
    return 0;
}
