// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

typedef struct {
    char *name;
    char **desc;
    size_t dcount;
    size_t dcap;
} Group;

int main() {
    Group *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *g_start = line + 6;
            char *space = strchr(g_start, ' ');
            if (space) {
                *space = '\0';
                char *d_start = space + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, g_start) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found == -1) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        arr = realloc(arr, cap * sizeof(Group));
                    }
                    arr[count].name = strdup(g_start);
                    arr[count].desc = NULL;
                    arr[count].dcount = 0;
                    arr[count].dcap = 0;
                    found = (int)count;
                    count++;
                }
                Group *g = &arr[found];
                if (g->dcount >= g->dcap) {
                    g->dcap = g->dcap == 0 ? 4 : g->dcap * 2;
                    g->desc = realloc(g->desc, g->dcap * sizeof(char *));
                }
                g->desc[g->dcount++] = strdup(d_start);
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *gA = line + 6;
            char *s1 = strchr(gA, ' ');
            if (s1) {
                *s1 = '\0';
                char *gB = s1 + 1;
                char *s2 = strchr(gB, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *newG = s2 + 1;
                    int idxA = -1, idxB = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, gA) == 0) idxA = (int)i;
                        if (strcmp(arr[i].name, gB) == 0) idxB = (int)i;
                    }
                    if (idxA != -1 && idxB != -1) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            arr = realloc(arr, cap * sizeof(Group));
                        }
                        arr[count].name = strdup(newG);
                        arr[count].dcount = arr[idxA].dcount + arr[idxB].dcount;
                        arr[count].dcap = arr[count].dcount;
                        arr[count].desc = malloc(arr[count].dcap * sizeof(char *));
                        size_t k = 0;
                        for (size_t i = 0; i < arr[idxA].dcount; i++) {
                            arr[count].desc[k++] = strdup(arr[idxA].desc[i]);
                        }
                        for (size_t i = 0; i < arr[idxB].dcount; i++) {
                            arr[count].desc[k++] = strdup(arr[idxB].desc[i]);
                        }
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *g_start = line + 12;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, g_start) == 0) {
                    free(arr[i].name);
                    for (size_t j = 0; j < arr[i].dcount; j++) {
                        free(arr[i].desc[j]);
                    }
                    free(arr[i].desc);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *g_start = line + 6;
            size_t cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, g_start) == 0) {
                    cnt = arr[i].dcount;
                    break;
                }
            }
            printf("%zu\n", cnt);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        for (size_t j = 0; j < arr[i].dcount; j++) {
            free(arr[i].desc[j]);
        }
        free(arr[i].desc);
    }
    free(arr);
    return 0;
}
