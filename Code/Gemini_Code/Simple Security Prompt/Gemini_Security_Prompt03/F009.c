// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char **descriptions;
    int desc_count;
} EventGroup;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

int main(void) {
    EventGroup *groups = NULL;
    int group_count = 0;
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
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            while (*p == ' ') p++;
            char *gname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *desc = p;
            int found = -1;
            for (int i = 0; i < group_count; i++) {
                if (groups[i].name && strcmp(groups[i].name, gname) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                groups = realloc(groups, (group_count + 1) * sizeof(EventGroup));
                groups[group_count].name = strdup(gname);
                groups[group_count].descriptions = malloc(sizeof(char *));
                groups[group_count].descriptions[0] = strdup(desc);
                groups[group_count].desc_count = 1;
                group_count++;
            } else {
                int dc = groups[found].desc_count;
                groups[found].descriptions = realloc(groups[found].descriptions, (dc + 1) * sizeof(char *));
                groups[found].descriptions[dc] = strdup(desc);
                groups[found].desc_count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ') p++;
            char *gA = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *gB = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *gNew = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idxA = -1, idxB = -1;
            for (int i = 0; i < group_count; i++) {
                if (groups[i].name && strcmp(groups[i].name, gA) == 0) idxA = i;
                if (groups[i].name && strcmp(groups[i].name, gB) == 0) idxB = i;
            }
            if (idxA != -1 && idxB != -1) {
                groups = realloc(groups, (group_count + 1) * sizeof(EventGroup));
                groups[group_count].name = strdup(gNew);
                int total_desc = groups[idxA].desc_count + groups[idxB].desc_count;
                groups[group_count].descriptions = malloc(total_desc * sizeof(char *));
                int k = 0;
                for (int i = 0; i < groups[idxA].desc_count; i++) {
                    groups[group_count].descriptions[k++] = strdup(groups[idxA].descriptions[i]);
                }
                for (int i = 0; i < groups[idxB].desc_count; i++) {
                    groups[group_count].descriptions[k++] = strdup(groups[idxB].descriptions[i]);
                }
                groups[group_count].desc_count = total_desc;
                group_count++;
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            while (*p == ' ') p++;
            char *gname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < group_count; i++) {
                if (groups[i].name && strcmp(groups[i].name, gname) == 0) {
                    free(groups[i].name);
                    for (int j = 0; j < groups[i].desc_count; j++) {
                        free(groups[i].descriptions[j]);
                    }
                    free(groups[i].descriptions);
                    groups[i].name = NULL;
                    groups[i].descriptions = NULL;
                    groups[i].desc_count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ') p++;
            char *gname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int count = 0;
            for (int i = 0; i < group_count; i++) {
                if (groups[i].name && strcmp(groups[i].name, gname) == 0) {
                    count = groups[i].desc_count;
                    break;
                }
            }
            printf("%d\n", count);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < group_count; i++) {
                if (groups[i].name) {
                    printf("%s %d\n", groups[i].name, groups[i].desc_count);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < group_count; i++) {
        if (groups[i].name) {
            free(groups[i].name);
            for (int j = 0; j < groups[i].desc_count; j++) {
                free(groups[i].descriptions[j]);
            }
            free(groups[i].descriptions);
        }
    }
    free(groups);
    return 0;
}
