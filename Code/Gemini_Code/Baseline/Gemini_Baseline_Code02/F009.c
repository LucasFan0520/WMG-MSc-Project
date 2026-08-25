// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *group;
    char *desc;
} Ev;

int main() {
    Ev *events = NULL;
    int count = 0;
    char **groups = NULL;
    int g_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < g_count; i++) {
                    printf("%s\n", groups[i]);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "EVENT") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *grp = args;
                char *desc = p2 + 1;
                events = realloc(events, sizeof(Ev) * (count + 1));
                events[count].group = strdup(grp);
                events[count].desc = strdup(desc);
                count++;
                int g_found = 0;
                for (int i = 0; i < g_count; i++) {
                    if (strcmp(groups[i], grp) == 0) { g_found = 1; break; }
                }
                if (!g_found) {
                    groups = realloc(groups, sizeof(char*) * (g_count + 1));
                    groups[g_count] = strdup(grp);
                    g_count++;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *gA = args;
            char *p2 = strchr(gA, ' ');
            if (p2) {
                *p2 = '\0';
                char *gB = p2 + 1;
                char *p3 = strchr(gB, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *newG = p3 + 1;
                    int cur_count = count;
                    for (int i = 0; i < cur_count; i++) {
                        if (strcmp(events[i].group, gA) == 0 || strcmp(events[i].group, gB) == 0) {
                            events = realloc(events, sizeof(Ev) * (count + 1));
                            events[count].group = strdup(newG);
                            events[count].desc = strdup(events[i].desc);
                            count++;
                        }
                    }
                    int g_found = 0;
                    for (int i = 0; i < g_count; i++) {
                        if (strcmp(groups[i], newG) == 0) { g_found = 1; break; }
                    }
                    if (!g_found) {
                        groups = realloc(groups, sizeof(char*) * (g_count + 1));
                        groups[g_count] = strdup(newG);
                        g_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *grp = args;
            int i = 0;
            while (i < count) {
                if (strcmp(events[i].group, grp) == 0) {
                    free(events[i].group);
                    free(events[i].desc);
                    for (int j = i; j < count - 1; j++) {
                        events[j] = events[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
            if (count == 0) { free(events); events = NULL; }
            else { events = realloc(events, sizeof(Ev) * count); }
            for (int k = 0; k < g_count; k++) {
                if (strcmp(groups[k], grp) == 0) {
                    free(groups[k]);
                    for (int j = k; j < g_count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    g_count--;
                    if (g_count == 0) { free(groups); groups = NULL; }
                    else { groups = realloc(groups, sizeof(char*) * g_count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *grp = args;
            int c = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(events[i].group, grp) == 0) c++;
            }
            printf("%d\n", c);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(events[i].group);
        free(events[i].desc);
    }
    free(events);
    for (int i = 0; i < g_count; i++) {
        free(groups[i]);
    }
    free(groups);
    return 0;
}
