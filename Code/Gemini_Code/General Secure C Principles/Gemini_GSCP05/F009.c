// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *description;
} Event;

typedef struct {
    char *name;
    Event *events;
    int event_count;
    int event_capacity;
} Group;

int main(void) {
    Group *groups = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *group_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *desc_start = p + 1;
                while (*desc_start == ' ') desc_start++;
                int g_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(groups[i].name, group_start) == 0) {
                        g_idx = i;
                        break;
                    }
                }
                if (g_idx == -1) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Group *new_g = realloc(groups, capacity * sizeof(Group));
                        if (new_g) groups = new_g;
                    }
                    if (count < capacity) {
                        groups[count].name = strdup(group_start);
                        groups[count].events = NULL;
                        groups[count].event_count = 0;
                        groups[count].event_capacity = 0;
                        g_idx = count;
                        count++;
                    }
                }
                if (g_idx != -1) {
                    Group *g = &groups[g_idx];
                    if (g->event_count >= g->event_capacity) {
                        g->event_capacity = g->event_capacity == 0 ? 4 : g->event_capacity * 2;
                        Event *new_e = realloc(g->events, g->event_capacity * sizeof(Event));
                        if (new_e) g->events = new_e;
                    }
                    if (g->event_count < g->event_capacity) {
                        g->events[g->event_count].description = strdup(desc_start);
                        g->event_count++;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *gA = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *gB = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *newG = p + 1;
                    while (*newG == ' ') newG++;
                    int idxA = -1, idxB = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(groups[i].name, gA) == 0) idxA = i;
                        if (strcmp(groups[i].name, gB) == 0) idxB = i;
                    }
                    if (idxA != -1 && idxB != -1) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Group *new_g = realloc(groups, capacity * sizeof(Group));
                            if (new_g) groups = new_g;
                        }
                        if (count < capacity) {
                            groups[count].name = strdup(newG);
                            int total_events = groups[idxA].event_count + groups[idxB].event_count;
                            groups[count].events = malloc(total_events * sizeof(Event));
                            groups[count].event_count = total_events;
                            groups[count].event_capacity = total_events;
                            int e_idx = 0;
                            for (int i = 0; i < groups[idxA].event_count; i++) {
                                groups[count].events[e_idx++].description = strdup(groups[idxA].events[i].description);
                            }
                            for (int i = 0; i < groups[idxB].event_count; i++) {
                                groups[count].events[e_idx++].description = strdup(groups[idxB].events[i].description);
                            }
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *g_name = line + 12;
            while (*g_name == ' ') g_name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(groups[i].name, g_name) == 0) {
                    free(groups[i].name);
                    for (int j = 0; j < groups[i].event_count; j++) {
                        free(groups[i].events[j].description);
                    }
                    free(groups[i].events);
                    for (int j = i; j < count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *g_name = line + 6;
            while (*g_name == ' ') g_name++;
            int res = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(groups[i].name, g_name) == 0) {
                    res = groups[i].event_count;
                    break;
                }
            }
            printf("%d\n", res);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", groups[i].name, groups[i].event_count);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(groups[i].name);
        for (int j = 0; j < groups[i].event_count; j++) {
            free(groups[i].events[j].description);
        }
        free(groups[i].events);
    }
    free(groups);
    return 0;
}
