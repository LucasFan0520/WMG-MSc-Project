// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char **descriptions;
    size_t desc_count;
    size_t desc_capacity;
} Group;

int main(void) {
    Group *groups = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "EVENT") == 0) {
            char *gname = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *desc = p;

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found == -1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Group *new_groups = realloc(groups, capacity * sizeof(Group));
                    if (!new_groups) exit(1);
                    groups = new_groups;
                }
                groups[count].name = strdup(gname);
                if (!groups[count].name) exit(1);
                groups[count].descriptions = NULL;
                groups[count].desc_count = 0;
                groups[count].desc_capacity = 0;
                found = (int)count;
                count++;
            }

            Group *g = &groups[found];
            if (g->desc_count >= g->desc_capacity) {
                g->desc_capacity = g->desc_capacity == 0 ? 4 : g->desc_capacity * 2;
                char **new_desc = realloc(g->descriptions, g->desc_capacity * sizeof(char *));
                if (!new_desc) exit(1);
                g->descriptions = new_desc;
            }
            g->descriptions[g->desc_count] = strdup(desc);
            if (!g->descriptions[g->desc_count]) exit(1);
            g->desc_count++;
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *gA = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *gB = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *newG = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int idxA = -1, idxB = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gA) == 0) idxA = (int)i;
                if (strcmp(groups[i].name, gB) == 0) idxB = (int)i;
            }

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Group *new_groups = realloc(groups, capacity * sizeof(Group));
                if (!new_groups) exit(1);
                groups = new_groups;
            }

            groups[count].name = strdup(newG);
            if (!groups[count].name) exit(1);
            groups[count].descriptions = NULL;
            groups[count].desc_count = 0;
            groups[count].desc_capacity = 0;

            Group *gNew = &groups[count];

            if (idxA != -1) {
                Group *ga = &groups[idxA];
                for (size_t i = 0; i < ga->desc_count; i++) {
                    if (gNew->desc_count >= gNew->desc_capacity) {
                        gNew->desc_capacity = gNew->desc_capacity == 0 ? 4 : gNew->desc_capacity * 2;
                        char **nd = realloc(gNew->descriptions, gNew->desc_capacity * sizeof(char *));
                        if (!nd) exit(1);
                        gNew->descriptions = nd;
                    }
                    gNew->descriptions[gNew->desc_count] = strdup(ga->descriptions[i]);
                    if (!gNew->descriptions[gNew->desc_count]) exit(1);
                    gNew->desc_count++;
                }
            }

            if (idxB != -1) {
                Group *gb = &groups[idxB];
                for (size_t i = 0; i < gb->desc_count; i++) {
                    if (gNew->desc_count >= gNew->desc_capacity) {
                        gNew->desc_capacity = gNew->desc_capacity == 0 ? 4 : gNew->desc_capacity * 2;
                        char **nd = realloc(gNew->descriptions, gNew->desc_capacity * sizeof(char *));
                        if (!nd) exit(1);
                        gNew->descriptions = nd;
                    }
                    gNew->descriptions[gNew->desc_count] = strdup(gb->descriptions[i]);
                    if (!gNew->descriptions[gNew->desc_count]) exit(1);
                    gNew->desc_count++;
                }
            }
            count++;
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *gname = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    free(groups[i].name);
                    for (size_t j = 0; j < groups[i].desc_count; j++) {
                        free(groups[i].descriptions[j]);
                    }
                    free(groups[i].descriptions);
                    for (size_t j = i; j < count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *gname = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            size_t cval = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    cval = groups[i].desc_count;
                    break;
                }
            }
            printf("%zu\n", cval);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", groups[i].name, groups[i].desc_count);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(groups[i].name);
        for (size_t j = 0; j < groups[i].desc_count; j++) {
            free(groups[i].descriptions[j]);
        }
        free(groups[i].descriptions);
    }
    free(groups);
    free(line);
    return 0;
}
