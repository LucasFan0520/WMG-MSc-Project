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
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *group;
    char *description;
} Event;

int main() {
    Event *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '	' || *p == '\n' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *group = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *description = p;
            size_t dl = strlen(description);
            while (dl > 0 && (description[dl-1] == '\n' || description[dl-1] == '\n')) {
                description[dl-1] = '\0';
                dl--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Event *narr = realloc(arr, cap * sizeof(Event));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].group = mystrdup(group);
                arr[count].description = mystrdup(description);
                count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *groupA = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *groupB = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *newGroup = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int cur_count = count;
            for (int i = 0; i < cur_count; i++) {
                if (strcmp(arr[i].group, groupA) == 0 || strcmp(arr[i].group, groupB) == 0) {
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        Event *narr = realloc(arr, cap * sizeof(Event));
                        if (narr) arr = narr;
                    }
                    if (count < cap) {
                        arr[count].group = mystrdup(newGroup);
                        arr[count].description = mystrdup(arr[i].description);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *group = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; ) {
                if (strcmp(arr[i].group, group) == 0) {
                    free(arr[i].group);
                    free(arr[i].description);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *group = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int cnt = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].group, group) == 0) {
                    cnt++;
                }
            }
            printf("%d\n", cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            char **seen = NULL;
            int seen_cnt = 0;
            for (int i = 0; i < count; i++) {
                int found = 0;
                for (int j = 0; j < seen_cnt; j++) {
                    if (strcmp(seen[j], arr[i].group) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", arr[i].group);
                    seen = realloc(seen, (seen_cnt + 1) * sizeof(char *));
                    if (seen) {
                        seen[seen_cnt] = arr[i].group;
                        seen_cnt++;
                    }
                }
            }
            free(seen);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].group);
        free(arr[i].description);
    }
    free(arr);
    return 0;
}
