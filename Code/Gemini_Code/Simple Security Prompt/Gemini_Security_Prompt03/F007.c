// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int time;
    char *label;
    char *note;
} Activity;

char *read_line(FILE *fp) {
    size_t size = 32;
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

int compare_activities(const void *a, const void *b) {
    Activity *actA = (Activity *)a;
    Activity *actB = (Activity *)b;
    if (actA->time != actB->time) {
        return (actA->time > actB->time) - (actA->time < actB->time);
    }
    return strcmp(actA->label, actB->label);
}

int main(void) {
    Activity *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *t_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            list = realloc(list, (count + 1) * sizeof(Activity));
            list[count].time = atoi(t_s);
            list[count].label = strdup(label);
            list[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *t_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int t_val = atoi(t_s);
            int write_idx = 0;
            for (int i = 0; i < count; i++) {
                if (list[i].time == t_val && strcmp(list[i].label, label) == 0) {
                    free(list[i].label);
                    free(list[i].note);
                } else {
                    list[write_idx++] = list[i];
                }
            }
            count = write_idx;
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int earliest_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].label, label) == 0) {
                    if (earliest_idx == -1 || list[i].time < list[earliest_idx].time) {
                        earliest_idx = i;
                    }
                }
            }
            if (earliest_idx != -1) {
                printf("%d %s %s\n", list[earliest_idx].time, list[earliest_idx].label, list[earliest_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(list, count, sizeof(Activity), compare_activities);
                for (int i = 0; i < count; i++) {
                    printf("%d %s %s\n", list[i].time, list[i].label, list[i].note);
                }
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(list[i].label);
                free(list[i].note);
            }
            free(list);
            list = NULL;
            count = 0;
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].label);
        free(list[i].note);
    }
    free(list);
    return 0;
}
