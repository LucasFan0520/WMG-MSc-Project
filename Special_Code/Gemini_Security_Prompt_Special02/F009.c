// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *group_name;
    char **descriptions;
    size_t desc_count;
    size_t desc_cap;
} Group;

int main() {
    Group *groups = NULL;
    size_t g_count = 0;
    size_t g_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *g1 = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            while (*p == ' ') p++;
            char *desc = p;
            int found = 0;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].group_name, g1) == 0) {
                    if (groups[i].desc_count >= groups[i].desc_cap) {
                        groups[i].desc_cap = groups[i].desc_cap == 0 ? 4 : groups[i].desc_cap * 2;
                        groups[i].descriptions = realloc(groups[i].descriptions, groups[i].desc_cap * sizeof(char *));
                    }
                    groups[i].descriptions[groups[i].desc_count++] = mystrdup(desc);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (g_count >= g_cap) {
                    g_cap = g_cap == 0 ? 4 : g_cap * 2;
                    groups = realloc(groups, g_cap * sizeof(Group));
                }
                groups[g_count].group_name = mystrdup(g1);
                groups[g_count].desc_count = 1;
                groups[g_count].desc_cap = 4;
                groups[g_count].descriptions = malloc(groups[g_count].desc_cap * sizeof(char *));
                groups[g_count].descriptions[0] = mystrdup(desc);
                g_count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ') p++;
            char *g2 = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *new_g = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            Group *src1 = NULL;
            Group *src2 = NULL;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].group_name, g1) == 0) src1 = &groups[i];
                if (strcmp(groups[i].group_name, g2) == 0) src2 = &groups[i];
            }
            if (src1 && src2) {
                if (g_count >= g_cap) {
                    g_cap = g_cap == 0 ? 4 : g_cap * 2;
                    groups = realloc(groups, g_cap * sizeof(Group));
                }
                groups[g_count].group_name = mystrdup(new_g);
                groups[g_count].desc_count = src1->desc_count + src2->desc_count;
                groups[g_count].desc_cap = groups[g_count].desc_count;
                groups[g_count].descriptions = malloc(groups[g_count].desc_cap * sizeof(char *));
                size_t idx = 0;
                for (size_t i = 0; i < src1->desc_count; i++) {
                    groups[g_count].descriptions[idx++] = mystrdup(src1->descriptions[i]);
                }
                for (size_t i = 0; i < src2->desc_count; i++) {
                    groups[g_count].descriptions[idx++] = mystrdup(src2->descriptions[i]);
                }
                g_count++;
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].group_name, g1) == 0) {
                    free(groups[i].group_name);
                    for (size_t j = 0; j < groups[i].desc_count; j++) {
                        free(groups[i].descriptions[j]);
                    }
                    free(groups[i].descriptions);
                    for (size_t j = i; j < g_count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    g_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int found = 0;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].group_name, g1) == 0) {
                    printf("%zu\n", groups[i].desc_count);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("0\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < g_count; i++) {
                printf("%s %zu\n", groups[i].group_name, groups[i].desc_count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < g_count; i++) {
        free(groups[i].group_name);
        for (size_t j = 0; j < groups[i].desc_count; j++) {
            free(groups[i].descriptions[j]);
        }
        free(groups[i].descriptions);
    }
    free(groups);
    return 0;
}
