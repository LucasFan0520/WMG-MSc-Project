// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *group;
    char *desc;
} Event;

int main(void) {
    Event *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    int duplicate = 0;
                    for (size_t j = 0; j < i; j++) {
                        if (strcmp(arr[j].group, arr[i].group) == 0) {
                            duplicate = 1;
                            break;
                        }
                    }
                    if (!duplicate) {
                        printf("%s\n", arr[i].group);
                    }
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = args;
            char *p2 = strchr(group, ' ');
            if (p2) {
                *p2 = '\0';
                char *desc = p2 + 1;
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    Event *narr = realloc(arr, ncap * sizeof(Event));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].group = strdup(group);
                    arr[count].desc = strdup(desc);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *gA = args;
            char *p2 = strchr(gA, ' ');
            if (p2) {
                *p2 = '\0';
                char *gB = p2 + 1;
                char *p3 = strchr(gB, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *nG = p3 + 1;
                    size_t old_count = count;
                    for (size_t i = 0; i < old_count; i++) {
                        if (strcmp(arr[i].group, gA) == 0 || strcmp(arr[i].group, gB) == 0) {
                            if (count >= cap) {
                                size_t ncap = cap * 2;
                                Event *narr = realloc(arr, ncap * sizeof(Event));
                                if (narr) {
                                    arr = narr;
                                    cap = ncap;
                                }
                            }
                            if (count < cap) {
                                arr[count].group = strdup(nG);
                                arr[count].desc = strdup(arr[i].desc);
                                count++;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *group = args;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].group, group) == 0) {
                    free(arr[i].group);
                    free(arr[i].desc);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *group = args;
            long cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].group, group) == 0) {
                    cnt++;
                }
            }
            printf("%ld\n", cnt);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].group);
        free(arr[i].desc);
    }
    free(arr);
    return 0;
}
