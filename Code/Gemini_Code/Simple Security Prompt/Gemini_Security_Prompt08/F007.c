// F007.c
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
    long long time;
    char *label;
    char *note;
} Activity;

int compare_act(const void *a, const void *b) {
    Activity *aa = (Activity *)a;
    Activity *ab = (Activity *)b;
    if (aa->time != ab->time) {
        return (aa->time > ab->time) - (aa->time < ab->time);
    }
    return strcmp(aa->label, ab->label);
}

int main(void) {
    Activity *list = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            char *time_str = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Activity *nb = realloc(list, cap * sizeof(Activity));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].time = atoll(time_str);
                list[count].label = strdup(label);
                list[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time_str = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            long long t = atoll(time_str);
            size_t i = 0;
            while (i < count) {
                if (list[i].time == t && strcmp(list[i].label, label) == 0) {
                    free(list[i].label);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            long long min_t = -1;
            size_t min_idx = 0;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].label, arg1) == 0) {
                    if (!found || list[i].time < min_t) {
                        min_t = list[i].time;
                        min_idx = i;
                        found = 1;
                    }
                }
            }
            if (found) {
                printf("%lld %s %s\n", list[min_idx].time, list[min_idx].label, list[min_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(list, count, sizeof(Activity), compare_act);
            for (size_t i = 0; i < count; i++) {
                printf("%lld %s %s\n", list[i].time, list[i].label, list[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(list[i].label);
                free(list[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].label);
        free(list[i].note);
    }
    free(list);
    return 0;
}
