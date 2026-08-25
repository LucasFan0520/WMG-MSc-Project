// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

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
    
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            char *grp = p;
            while (*grp == ' ') grp++;
            char *grp_end = grp;
            while (*grp_end && *grp_end != ' ') grp_end++;
            char *desc = grp_end;
            if (*grp_end != '\0') {
                *grp_end = '\0';
                desc++;
            }
            while (*desc == ' ') desc++;
            int g_found = 0;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i], grp) == 0) {
                    g_found = 1;
                    break;
                }
            }
            if (!g_found) {
                if (g_count >= g_capacity) {
                    g_capacity = g_capacity == 0 ? 4 : g_capacity * 2;
                    char **new_g = realloc(groups, g_capacity * sizeof(char *));
                    if (new_g) groups = new_g;
                }
                groups[g_count++] = mystrdup(grp);
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Event *new_e = realloc(events, capacity * sizeof(Event));
                if (new_e) events = new_e;
            }
            events[count].group = mystrdup(grp);
            events[count].description = mystrdup(desc);
            count++;
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *gA = p;
            while (*gA == ' ') gA++;
            char *gA_end = gA;
            while (*gA_end && *gA_end != ' ') gA_end++;
            if (*gA_end != '\0') {
                *gA_end = '\0';
                gA_end++;
            }
            char *gB = gA_end;
            while (*gB == ' ') gB++;
            char *gB_end = gB;
            while (*gB_end && *gB_end != ' ') gB_end++;
            char *gNew = gB_end;
            if (*gB_end != '\0') {
                *gB_end = '\0';
                gNew++;
            }
            while (*gNew == ' ') gNew++;
            char *gNew_end = gNew;
            while (*gNew_end && *gNew_end != ' ') gNew_end++;
            *gNew_end = '\0';
            int g_found = 0;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i], gNew) == 0) {
                    g_found = 1;
                    break;
                }
            }
            if (!g_found) {
                if (g_count >= g_capacity) {
                    g_capacity = g_capacity == 0 ? 4 : g_capacity * 2;
                    char **new_g = realloc(groups, g_capacity * sizeof(char *));
                    if (new_g) groups = new_g;
                }
                groups[g_count++] = mystrdup(gNew);
            }
            size_t old_count = count;
            for (size_t i = 0; i < old_count; i++) {
                if (strcmp(events[i].group, gA) == 0 || strcmp(events[i].group, gB) == 0) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Event *new_e = realloc(events, capacity * sizeof(Event));
                        if (new_e) events = new_e;
                    }
                    events[count].group = mystrdup(gNew);
                    events[count].description = mystrdup(events[i].description);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *grp = p;
            while (*grp == ' ') grp++;
            char *grp_end = grp;
            while (*grp_end && *grp_end != ' ') grp_end++;
            *grp_end = '\0';
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i], grp) == 0) {
                    free(groups[i]);
                    for (size_t j = i; j < g_count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    g_count--;
                    break;
                }
            }
            for (size_t i = 0; i < count; ) {
                if (strcmp(events[i].group, grp) == 0) {
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
            char *grp = p;
            while (*grp == ' ') grp++;
            char *grp_end = grp;
            while (*grp_end && *grp_end != ' ') grp_end++;
            *grp_end = '\0';
            int c = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(events[i].group, grp) == 0) {
                    c++;
                }
            }
            printf("%d\n", c);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < g_count; i++) {
                printf("%s\n", groups[i]);
            }
        }
        free(line);
    }
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
