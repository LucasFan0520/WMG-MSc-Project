// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *group;
    char *desc;
} Event;

int main(void) {
    Event *events = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *grp = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *desc = p;
            if (*grp != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Event *new_events = realloc(events, capacity * sizeof(Event));
                    if (new_events) events = new_events;
                }
                if (count < capacity) {
                    events[count].group = strdup(grp);
                    events[count].desc = strdup(desc);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *grpA = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *grpB = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *newGrp = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            size_t old_count = count;
            for (size_t i = 0; i < old_count; i++) {
                if (strcmp(events[i].group, grpA) == 0 || strcmp(events[i].group, grpB) == 0) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Event *new_events = realloc(events, capacity * sizeof(Event));
                        if (new_events) events = new_events;
                    }
                    if (count < capacity) {
                        events[count].group = strdup(newGrp);
                        events[count].desc = strdup(events[i].desc);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *grp = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            size_t idx = 0;
            while (idx < count) {
                if (strcmp(events[idx].group, grp) == 0) {
                    free(events[idx].group);
                    free(events[idx].desc);
                    for (size_t j = idx; j < count - 1; j++) {
                        events[j] = events[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *grp = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(events[i].group, grp) == 0) {
                    cnt++;
                }
            }
            printf("%lld\n", cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            char **seen = NULL;
            size_t seen_count = 0;
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < seen_count; j++) {
                    if (strcmp(seen[j], events[i].group) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    seen = realloc(seen, (seen_count + 1) * sizeof(char *));
                    seen[seen_count++] = events[i].group;
                    printf("%s\n", events[i].group);
                }
            }
            free(seen);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(events[i].group);
        free(events[i].desc);
    }
    free(events);
    return 0;
}
