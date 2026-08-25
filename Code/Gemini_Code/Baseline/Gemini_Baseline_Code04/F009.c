// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *desc;
} Event;

typedef struct {
    char *name;
    Event *events;
    int event_count;
} Group;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Group *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *p = line + 6;
            char *group = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *desc = p + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, group) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    arr = realloc(arr, (count + 1) * sizeof(Group));
                    arr[count].name = strdup(group);
                    arr[count].events = NULL;
                    arr[count].event_count = 0;
                    found = count;
                    count++;
                }
                int ec = arr[found].event_count;
                arr[found].events = realloc(arr[found].events, (ec + 1) * sizeof(Event));
                arr[found].events[ec].desc = strdup(desc);
                arr[found].event_count++;
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            char *groupA = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *groupB = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *newGroup = p + 1;
                    int idxA = -1, idxB = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, groupA) == 0) idxA = i;
                        if (strcmp(arr[i].name, groupB) == 0) idxB = i;
                    }
                    if (idxA != -1 && idxB != -1) {
                        arr = realloc(arr, (count + 1) * sizeof(Group));
                        arr[count].name = strdup(newGroup);
                        int total = arr[idxA].event_count + arr[idxB].event_count;
                        arr[count].events = malloc(total * sizeof(Event));
                        int k = 0;
                        for (int i = 0; i < arr[idxA].event_count; i++) {
                            arr[count].events[k++].desc = strdup(arr[idxA].events[i].desc);
                        }
                        for (int i = 0; i < arr[idxB].event_count; i++) {
                            arr[count].events[k++].desc = strdup(arr[idxB].events[i].desc);
                        }
                        arr[count].event_count = total;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *group = line + 12;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, group) == 0) {
                    free(arr[i].name);
                    for (int j = 0; j < arr[i].event_count; j++) {
                        free(arr[i].events[j].desc);
                    }
                    free(arr[i].events);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *group = line + 6;
            int ec = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, group) == 0) {
                    ec = arr[i].event_count;
                    break;
                }
            }
            printf("%d\n", ec);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int j = 0; j < arr[i].event_count; j++) {
            free(arr[i].events[j].desc);
        }
        free(arr[i].events);
    }
    free(arr);
    return 0;
}
