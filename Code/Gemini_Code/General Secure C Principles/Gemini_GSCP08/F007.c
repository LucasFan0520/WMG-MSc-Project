// F007.c
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
    long time;
    char *label;
    char *note;
} Activity;

int compare_activities(const void *a, const void *b) {
    const Activity *aa = (const Activity *)a;
    const Activity *bb = (const Activity *)b;
    if (aa->time < bb->time) return -1;
    if (aa->time > bb->time) return 1;
    return strcmp(aa->label, bb->label);
}

int main(void) {
    Activity *arr = NULL;
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
            if (strcmp(cmd, "SORT") == 0) {
                if (count > 0) {
                    qsort(arr, count, sizeof(Activity), compare_activities);
                    for (size_t i = 0; i < count; i++) {
                        printf("%ld %s %s\n", arr[i].time, arr[i].label, arr[i].note);
                    }
                }
            } else if (strcmp(cmd, "CLEAR") == 0) {
                for (size_t i = 0; i < count; i++) {
                    free(arr[i].label);
                    free(arr[i].note);
                }
                count = 0;
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *t_str = args;
            char *p2 = strchr(t_str, ' ');
            if (p2) {
                *p2 = '\0';
                char *label = p2 + 1;
                char *p3 = strchr(label, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    long t = strtol(t_str, NULL, 10);
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Activity *narr = realloc(arr, ncap * sizeof(Activity));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].time = t;
                        arr[count].label = strdup(label);
                        arr[count].note = strdup(note);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *t_str = args;
            char *p2 = strchr(t_str, ' ');
            if (p2) {
                *p2 = '\0';
                char *label = p2 + 1;
                long t = strtol(t_str, NULL, 10);
                size_t i = 0;
                while (i < count) {
                    if (arr[i].time == t && strcmp(arr[i].label, label) == 0) {
                        free(arr[i].label);
                        free(arr[i].note);
                        for (size_t j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = args;
            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].label, label) == 0) {
                    if (found_idx == -1 || arr[i].time < arr[found_idx].time) {
                        found_idx = (int)i;
                    }
                }
            }
            if (found_idx != -1) {
                printf("%ld %s %s\n", arr[found_idx].time, arr[found_idx].label, arr[found_idx].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].label);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
