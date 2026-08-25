// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *group;
    char *description;
} Event;

int main(void) {
    Event *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *desc = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Event *nb = realloc(list, cap * sizeof(Event));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].group = strdup(group);
                list[count].description = strdup(desc);
                count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *groupA = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *groupB = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *newGroup = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t current_count = count;
            for (size_t i = 0; i < current_count; i++) {
                if (strcmp(list[i].group, groupA) == 0 || strcmp(list[i].group, groupB) == 0) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Event *nb = realloc(list, cap * sizeof(Event));
                        if (nb) list = nb;
                    }
                    if (count < cap) {
                        list[count].group = strdup(newGroup);
                        list[count].description = strdup(list[i].description);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].group, arg1) == 0) {
                    free(list[i].group);
                    free(list[i].description);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int c = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].group, arg1) == 0) {
                    c++;
                }
            }
            printf("%d\n", c);
        } else if (strcmp(cmd, "REPORT") == 0) {
            char **seen = NULL;
            size_t s_count = 0;
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < s_count; j++) {
                    if (strcmp(seen[j], list[i].group) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", list[i].group);
                    seen = realloc(seen, (s_count + 1) * sizeof(char *));
                    if (seen) {
                        seen[s_count] = list[i].group;
                        s_count++;
                    }
                }
            }
            free(seen);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].group);
        free(list[i].description);
    }
    free(list);
    return 0;
}
