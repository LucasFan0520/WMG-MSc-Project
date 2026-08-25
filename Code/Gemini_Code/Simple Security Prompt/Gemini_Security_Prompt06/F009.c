// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
} Group;

typedef struct {
    char *group;
    char *description;
} Event;

char *read_line(FILE *f) {
    size_t size = 32;
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Group *groups = NULL;
    int g_count = 0;
    Event *events = NULL;
    int e_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < g_count; i++) {
                    printf("%s\n", groups[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "EVENT") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *group = rest;
                char *desc = p2 + 1;
                int g_found = -1;
                for (int i = 0; i < g_count; i++) {
                    if (strcmp(groups[i].name, group) == 0) {
                        g_found = i;
                        break;
                    }
                }
                if (g_found == -1) {
                    groups = realloc(groups, (g_count + 1) * sizeof(Group));
                    groups[g_count].name = strdup(group);
                    g_count++;
                }
                events = realloc(events, (e_count + 1) * sizeof(Event));
                events[e_count].group = strdup(group);
                events[e_count].description = strdup(desc);
                e_count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *gA = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *gB = rest2;
                    char *newG = p3 + 1;
                    int g_found = -1;
                    for (int i = 0; i < g_count; i++) {
                        if (strcmp(groups[i].name, newG) == 0) {
                            g_found = i;
                            break;
                        }
                    }
                    if (g_found == -1) {
                        groups = realloc(groups, (g_count + 1) * sizeof(Group));
                        groups[g_count].name = strdup(newG);
                        g_count++;
                    }
                    int current_e_count = e_count;
                    for (int i = 0; i < current_e_count; i++) {
                        if (strcmp(events[i].group, gA) == 0 || strcmp(events[i].group, gB) == 0) {
                            events = realloc(events, (e_count + 1) * sizeof(Event));
                            events[e_count].group = strdup(newG);
                            events[e_count].description = strdup(events[i].description);
                            e_count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *group = rest;
            int g_found = -1;
            for (int i = 0; i < g_count; i++) {
                if (strcmp(groups[i].name, group) == 0) {
                    g_found = i;
                    break;
                }
            }
            if (g_found != -1) {
                free(groups[g_found].name);
                for (int i = g_found; i < g_count - 1; i++) {
                    groups[i] = groups[i + 1];
                }
                g_count--;
                if (g_count == 0) {
                    free(groups);
                    groups = NULL;
                } else {
                    groups = realloc(groups, g_count * sizeof(Group));
                }
                int i = 0;
                while (i < e_count) {
                    if (strcmp(events[i].group, group) == 0) {
                        free(events[i].group);
                        free(events[i].description);
                        for (int j = i; j < e_count - 1; j++) {
                            events[j] = events[j + 1];
                        }
                        e_count--;
                    } else {
                        i++;
                    }
                }
                if (e_count == 0) {
                    free(events);
                    events = NULL;
                } else {
                    events = realloc(events, e_count * sizeof(Event));
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *group = rest;
            int c = 0;
            for (int i = 0; i < e_count; i++) {
                if (strcmp(events[i].group, group) == 0) {
                    c++;
                }
            }
            printf("%d\n", c);
        }
        free(line);
    }
    for (int i = 0; i < g_count; i++) {
        free(groups[i].name);
    }
    free(groups);
    for (int i = 0; i < e_count; i++) {
        free(events[i].group);
        free(events[i].description);
    }
    free(events);
    return 0;
}