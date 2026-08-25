// F009.c
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
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *group;
    char *description;
} Event;

typedef struct {
    char *name;
} Group;

int main(void) {
    Event *events = NULL;
    size_t ev_count = 0;
    size_t ev_cap = 0;
    Group *groups = NULL;
    size_t g_count = 0;
    size_t g_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *p = line + 6;
            char *group = p;
            while (*p && *p != ' ') p++;
            char *desc = "";
            if (*p == ' ') {
                *p = '\0';
                desc = p + 1;
            }
            int g_found = 0;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].name, group) == 0) {
                    g_found = 1;
                    break;
                }
            }
            if (!g_found) {
                if (g_count >= g_cap) {
                    g_cap = g_cap == 0 ? 4 : g_cap * 2;
                    Group *nb = realloc(groups, g_cap * sizeof(Group));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    groups = nb;
                }
                groups[g_count].name = safe_dup(group);
                g_count++;
            }
            if (ev_count >= ev_cap) {
                ev_cap = ev_cap == 0 ? 4 : ev_cap * 2;
                Event *nb = realloc(events, ev_cap * sizeof(Event));
                if (!nb) {
                    free(line);
                    break;
                }
                events = nb;
            }
            events[ev_count].group = safe_dup(group);
            events[ev_count].description = safe_dup(desc);
            ev_count++;
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            char *groupA = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *groupB = p;
            while (*p && *p != ' ') p++;
            char *newGroup = "";
            if (*p == ' ') {
                *p = '\0';
                newGroup = p + 1;
            }
            int g_found = 0;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].name, newGroup) == 0) {
                    g_found = 1;
                    break;
                }
            }
            if (!g_found) {
                if (g_count >= g_cap) {
                    g_cap = g_cap == 0 ? 4 : g_cap * 2;
                    Group *nb = realloc(groups, g_cap * sizeof(Group));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    groups = nb;
                }
                groups[g_count].name = safe_dup(newGroup);
                g_count++;
            }
            size_t current_ev_count = ev_count;
            for (size_t i = 0; i < current_ev_count; i++) {
                if (strcmp(events[i].group, groupA) == 0 || strcmp(events[i].group, groupB) == 0) {
                    if (ev_count >= ev_cap) {
                        ev_cap = ev_cap * 2;
                        Event *nb = realloc(events, ev_cap * sizeof(Event));
                        if (!nb) {
                            break;
                        }
                        events = nb;
                    }
                    events[ev_count].group = safe_dup(newGroup);
                    events[ev_count].description = safe_dup(events[i].description);
                    ev_count++;
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *group = line + 12;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].name, group) == 0) {
                    free(groups[i].name);
                    for (size_t j = i; j < g_count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    g_count--;
                    break;
                }
            }
            size_t i = 0;
            while (i < ev_count) {
                if (strcmp(events[i].group, group) == 0) {
                    free(events[i].group);
                    free(events[i].description);
                    for (size_t j = i; j < ev_count - 1; j++) {
                        events[j] = events[j + 1];
                    }
                    ev_count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *group = line + 6;
            size_t c = 0;
            for (size_t i = 0; i < ev_count; i++) {
                if (strcmp(events[i].group, group) == 0) {
                    c++;
                }
            }
            printf("%zu\n", c);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < g_count; i++) {
                printf("%s\n", groups[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < ev_count; i++) {
        free(events[i].group);
        free(events[i].description);
    }
    free(events);
    for (size_t i = 0; i < g_count; i++) {
        free(groups[i].name);
    }
    free(groups);
    return 0;
}
