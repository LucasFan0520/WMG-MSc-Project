// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
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
    char *old_val;
    char *new_val;
} RenameRule;

int main(void) {
    RenameRule *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
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
        if (strcmp(cmd, "ADD") == 0) {
            char *old_v = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *new_v = p;
            size_t nvlen = strlen(new_v);
            while (nvlen > 0 && (new_v[nvlen-1] == '\n' || new_v[nvlen-1] == '\n')) {
                new_v[nvlen-1] = '\0';
                nvlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                RenameRule *nitems = realloc(items, cap * sizeof(RenameRule));
                if (nitems) items = nitems;
            }
            items[count].old_val = mystrdup(old_v);
            items[count].new_val = mystrdup(new_v);
            count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = p;
            size_t tlen = strlen(text);
            while (tlen > 0 && (text[tlen-1] == '\n' || text[tlen-1] == '\n')) {
                text[tlen-1] = '\0';
                tlen--;
            }
            char *res = NULL;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].old_val, text) == 0) {
                    res = items[i].new_val;
                    break;
                }
            }
            if (res) {
                printf("%s\n", res);
            } else {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old_v = p;
            size_t ovlen = strlen(old_v);
            while (ovlen > 0 && (old_v[ovlen-1] == '\n' || old_v[ovlen-1] == '\n')) {
                old_v[ovlen-1] = '\0';
                ovlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].old_val, old_v) == 0) {
                    free(items[i].old_val);
                    free(items[i].new_val);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *j_s = p;
            size_t jlen = strlen(j_s);
            while (jlen > 0 && (j_s[jlen-1] == '\n' || j_s[jlen-1] == '\n')) {
                j_s[jlen-1] = '\0';
                jlen--;
            }
            int idx_i = atoi(i_s);
            int idx_j = atoi(j_s);
            if (idx_i >= 0 && (size_t)idx_i < count && idx_j >= 0 && (size_t)idx_j < count) {
                RenameRule tmp = items[idx_i];
                items[idx_i] = items[idx_j];
                items[idx_j] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s -> %s\n", items[i].old_val, items[i].new_val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].old_val);
        free(items[i].new_val);
    }
    free(items);
    return 0;
}
