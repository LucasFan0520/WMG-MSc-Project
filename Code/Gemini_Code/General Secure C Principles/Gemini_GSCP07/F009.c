// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *group;
    char *desc;
} EventRec;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    EventRec *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            while (*p == ' ') p++;
            char *group = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *desc = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                EventRec *nl = realloc(list, capacity * sizeof(EventRec));
                if (!nl) break;
                list = nl;
            }
            list[count].group = strdup(group);
            list[count].desc = strdup(desc);
            count++;
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ') p++;
            char *groupA = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *groupB = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *newGroup = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t initial_count = count;
            for (size_t i = 0; i < initial_count; i++) {
                if (strcmp(list[i].group, groupA) == 0 || strcmp(list[i].group, groupB) == 0) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        EventRec *nl = realloc(list, capacity * sizeof(EventRec));
                        if (!nl) break;
                        list = nl;
                    }
                    list[count].group = strdup(newGroup);
                    list[count].desc = strdup(list[i].desc);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            while (*p == ' ') p++;
            char *group = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].group, group) == 0) {
                    free(list[i].group);
                    free(list[i].desc);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ') p++;
            char *group = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int gcount = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].group, group) == 0) {
                    gcount++;
                }
            }
            printf("%d\n", gcount);
        } else if (strcmp(cmd, "REPORT") == 0) {
            char **reported = malloc(count * sizeof(char *));
            size_t rep_count = 0;
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < rep_count; j++) {
                    if (strcmp(reported[j], list[i].group) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    int gcount = 0;
                    for (size_t j = 0; j < count; j++) {
                        if (strcmp(list[j].group, list[i].group) == 0) {
                            gcount++;
                        }
                    }
                    printf("%s %d\n", list[i].group, gcount);
                    reported[rep_count++] = list[i].group;
                }
            }
            free(reported);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].group);
        free(list[i].desc);
    }
    free(list);
    return 0;
}
