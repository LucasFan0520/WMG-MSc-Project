// F009.c
#define _GNU_SOURCE
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
    Group *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *grp = p;
            while (*p && *p != ' ') p++;
            char *desc = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                desc = p;
            }
            int g_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, grp) == 0) {
                    g_idx = (int)i;
                    break;
                }
            }
            if (g_idx == -1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Group *tmp = realloc(arr, capacity * sizeof(Group));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(grp);
                arr[count].descriptions = NULL;
                arr[count].desc_count = 0;
                arr[count].desc_capacity = 0;
                if (!arr[count].name) exit(1);
                g_idx = (int)count;
                count++;
            }
            Group *g = &arr[g_idx];
            if (g->desc_count >= g->desc_capacity) {
                g->desc_capacity = g->desc_capacity == 0 ? 4 : g->desc_capacity * 2;
                char **tmp = realloc(g->descriptions, g->desc_capacity * sizeof(char *));
                if (!tmp) exit(1);
                g->descriptions = tmp;
            }
            g->descriptions[g->desc_count] = strdup(desc);
            if (!g->descriptions[g->desc_count]) exit(1);
            g->desc_count++;
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *grpA = p;
            while (*p && *p != ' ') p++;
            char *grpB = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                grpB = p;
            }
            while (*p && *p != ' ') p++;
            char *newGrp = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                newGrp = p;
            }
            int idxA = -1, idxB = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, grpA) == 0) idxA = (int)i;
                if (strcmp(arr[i].name, grpB) == 0) idxB = (int)i;
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Group *tmp = realloc(arr, capacity * sizeof(Group));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].name = strdup(newGrp);
            arr[count].descriptions = NULL;
            arr[count].desc_count = 0;
            arr[count].desc_capacity = 0;
            if (!arr[count].name) exit(1);
            Group *g = &arr[count];
            if (idxA != -1) {
                Group *gA = &arr[idxA];
                for (size_t i = 0; i < gA->desc_count; i++) {
                    if (g->desc_count >= g->desc_capacity) {
                        g->desc_capacity = g->desc_capacity == 0 ? 4 : g->desc_capacity * 2;
                        char **tmp = realloc(g->descriptions, g->desc_capacity * sizeof(char *));
                        if (!tmp) exit(1);
                        g->descriptions = tmp;
                    }
                    g->descriptions[g->desc_count] = strdup(gA->descriptions[i]);
                    if (!g->descriptions[g->desc_count]) exit(1);
                    g->desc_count++;
                }
            }
            if (idxB != -1) {
                Group *gB = &arr[idxB];
                for (size_t i = 0; i < gB->desc_count; i++) {
                    if (g->desc_count >= g->desc_capacity) {
                        g->desc_capacity = g->desc_capacity == 0 ? 4 : g->desc_capacity * 2;
                        char **tmp = realloc(g->descriptions, g->desc_capacity * sizeof(char *));
                        if (!tmp) exit(1);
                        g->descriptions = tmp;
                    }
                    g->descriptions[g->desc_count] = strdup(gB->descriptions[i]);
                    if (!g->descriptions[g->desc_count]) exit(1);
                    g->desc_count++;
                }
            }
            count++;
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *grp = line + 12;
            while (*grp == ' ') grp++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, grp) == 0) {
                    free(arr[i].name);
                    for (size_t j = 0; j < arr[i].desc_count; j++) {
                        free(arr[i].descriptions[j]);
                    }
                    free(arr[i].descriptions);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *grp = line + 6;
            while (*grp == ' ') grp++;
            size_t c_val = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, grp) == 0) {
                    c_val = arr[i].desc_count;
                    break;
                }
            }
            printf("%zu\n", c_val);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        for (size_t j = 0; j < arr[i].desc_count; j++) {
            free(arr[i].descriptions[j]);
        }
        free(arr[i].descriptions);
    }
    free(arr);
    free(line);
    return 0;
}
