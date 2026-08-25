/* F009.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *group;
    char *description;
} Event;

int main(void) {
    Event *events = NULL;
    size_t count = 0;
    size_t capacity = 0;

    char **groups = NULL;
    size_t g_count = 0;
    size_t g_capacity = 0;

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;

        if (strcmp(cmd, "EVENT") == 0) {
            char *group_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *desc_str = p;

            int g_found = 0;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i], group_str) == 0) {
                    g_found = 1;
                    break;
                }
            }
            if (!g_found) {
                if (g_count >= g_capacity) {
                    size_t new_cap = g_capacity == 0 ? 4 : g_capacity * 2;
                    char **temp = realloc(groups, new_cap * sizeof(char *));
                    if (!temp) break;
                    groups = temp;
                    g_capacity = new_cap;
                }
                groups[g_count] = strdup(group_str);
                if (groups[g_count]) g_count++;
            }

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Event *temp = realloc(events, new_cap * sizeof(Event));
                if (!temp) break;
                events = temp;
                capacity = new_cap;
            }
            events[count].group = strdup(group_str);
            events[count].description = strdup(desc_str);
            if (events[count].group && events[count].description) {
                count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *gA = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *gB = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *newG = p;

            int g_found = 0;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i], newG) == 0) {
                    g_found = 1;
                    break;
                }
            }
            if (!g_found) {
                if (g_count >= g_capacity) {
                    size_t new_cap = g_capacity == 0 ? 4 : g_capacity * 2;
                    char **temp = realloc(groups, new_cap * sizeof(char *));
                    if (!temp) break;
                    groups = temp;
                    g_capacity = new_cap;
                }
                groups[g_count] = strdup(newG);
                if (groups[g_count]) g_count++;
            }

            size_t initial_count = count;
            for (size_t i = 0; i < initial_count; i++) {
                if (strcmp(events[i].group, gA) == 0 || strcmp(events[i].group, gB) == 0) {
                    if (count >= capacity) {
                        size_t new_cap = capacity * 2;
                        Event *temp = realloc(events, new_cap * sizeof(Event));
                        if (!temp) break;
                        events = temp;
                        capacity = new_cap;
                    }
                    events[count].group = strdup(newG);
                    events[count].description = strdup(events[i].description);
                    if (events[count].group && events[count].description) {
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *g_str = p;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i], g_str) == 0) {
                    free(groups[i]);
                    for (size_t j = i; j < g_count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    g_count--;
                    break;
                }
            }
            for (size_t i = 0; i < count; ) {
                if (strcmp(events[i].group, g_str) == 0) {
                    free(events[i].group);
                    free(events[i].description);
                    for (size_t j = i; j < count - 1; j++) {
                        events[j] = events[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *g_str = p;
            size_t amt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(events[i].group, g_str) == 0) {
                    amt++;
                }
            }
            printf("%zu\n", amt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < g_count; i++) {
                printf("%s\n", groups[i]);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(events[i].group);
        free(events[i].description);
    }
    free(events);
    for (size_t i = 0; i < g_count; i++) {
        free(groups[i]);
    }
    free(groups);
    return 0;
}
