// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *group;
    char *description;
} Event;

int main() {
    Event *events = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *p = line + 6;
            char *grp = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *desc = p + 1;
                events = realloc(events, (count + 1) * sizeof(Event));
                events[count].group = strdup(grp);
                events[count].description = strdup(desc);
                count++;
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char gA[128], gB[128], nG[128];
            if (sscanf(line + 6, "%127s %127s %127s", gA, gB, nG) == 3) {
                int current_count = count;
                for (int i = 0; i < current_count; i++) {
                    if (strcmp(events[i].group, gA) == 0 || strcmp(events[i].group, gB) == 0) {
                        events = realloc(events, (count + 1) * sizeof(Event));
                        events[count].group = strdup(nG);
                        events[count].description = strdup(events[i].description);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *grp = line + 12;
            int i = 0;
            while (i < count) {
                if (strcmp(events[i].group, grp) == 0) {
                    free(events[i].group);
                    free(events[i].description);
                    for (int j = i; j < count - 1; j++) {
                        events[j] = events[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
            events = realloc(events, count * sizeof(Event));
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *grp = line + 6;
            int c = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(events[i].group, grp) == 0) {
                    c++;
                }
            }
            printf("%d\n", c);
        } else if (strcmp(line, "REPORT") == 0) {
            char **printed = NULL;
            int p_count = 0;
            for (int i = 0; i < count; i++) {
                int found = 0;
                for (int j = 0; j < p_count; j++) {
                    if (strcmp(printed[j], events[i].group) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", events[i].group);
                    printed = realloc(printed, (p_count + 1) * sizeof(char *));
                    printed[p_count] = events[i].group;
                    p_count++;
                }
            }
            free(printed);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(events[i].group);
        free(events[i].description);
    }
    free(events);
    return 0;
}
