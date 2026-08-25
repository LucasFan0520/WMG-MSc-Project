// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char **events;
    int event_count;
    int event_capacity;
} Group;

int main() {
    Group *groups = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", groups[i].name);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "EVENT") == 0) {
            char gname[256];
            int n2 = 0;
            if (sscanf(p, "%255s%n", gname, &n2) == 1) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        groups = realloc(groups, capacity * sizeof(Group));
                    }
                    groups[count].name = strdup(gname);
                    groups[count].events = NULL;
                    groups[count].event_count = 0;
                    groups[count].event_capacity = 0;
                    found = count;
                    count++;
                }
                if (groups[found].event_count >= groups[found].event_capacity) {
                    groups[found].event_capacity = groups[found].event_capacity == 0 ? 4 : groups[found].event_capacity * 2;
                    groups[found].events = realloc(groups[found].events, groups[found].event_capacity * sizeof(char *));
                }
                groups[found].events[groups[found].event_count++] = strdup(p);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char gA[256], gB[256], nG[256];
            if (sscanf(p, "%255s %255s %255s", gA, gB, nG) == 3) {
                int fA = -1, fB = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(groups[i].name, gA) == 0) fA = i;
                    if (strcmp(groups[i].name, gB) == 0) fB = i;
                }
                if (fA != -1 && fB != -1) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        groups = realloc(groups, capacity * sizeof(Group));
                    }
                    groups[count].name = strdup(nG);
                    groups[count].event_count = groups[fA].event_count + groups[fB].event_count;
                    groups[count].event_capacity = groups[count].event_count;
                    groups[count].events = malloc(groups[count].event_capacity * sizeof(char *));
                    int idx = 0;
                    for (int i = 0; i < groups[fA].event_count; i++) {
                        groups[count].events[idx++] = strdup(groups[fA].events[i]);
                    }
                    for (int i = 0; i < groups[fB].event_count; i++) {
                        groups[count].events[idx++] = strdup(groups[fB].events[i]);
                    }
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char gname[256];
            if (sscanf(p, "%255s", gname) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(groups[found].name);
                    for (int j = 0; j < groups[found].event_count; j++) {
                        free(groups[found].events[j]);
                    }
                    free(groups[found].events);
                    for (int i = found; i < count - 1; i++) {
                        groups[i] = groups[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char gname[256];
            if (sscanf(p, "%255s", gname) == 1) {
                int amt = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        amt = groups[i].event_count;
                        break;
                    }
                }
                printf("%d\n", amt);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(groups[i].name);
        for (int j = 0; j < groups[i].event_count; j++) {
            free(groups[i].events[j]);
        }
        free(groups[i].events);
    }
    free(groups);
    return 0;
}
