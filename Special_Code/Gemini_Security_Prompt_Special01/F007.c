// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    long long time;
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
    Activity *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *tm_s = p;
            while (*tm_s == ' ') tm_s++;
            char *tm_end = tm_s;
            while (*tm_end && *tm_end != ' ') tm_end++;
            char *lbl = tm_end;
            if (*tm_end != '\0') {
                *tm_end = '\0';
                lbl++;
            }
            while (*lbl == ' ') lbl++;
            char *lbl_end = lbl;
            while (*lbl_end && *lbl_end != ' ') lbl_end++;
            char *note = lbl_end;
            if (*lbl_end != '\0') {
                *lbl_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            long long tm = atoll(tm_s);
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Activity *new_list = realloc(list, capacity * sizeof(Activity));
                if (!new_list) {
                    free(line);
                    break;
                }
                list = new_list;
            }
            list[count].time = tm;
            list[count].label = mystrdup(lbl);
            list[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *tm_s = p;
            while (*tm_s == ' ') tm_s++;
            char *tm_end = tm_s;
            while (*tm_end && *tm_end != ' ') tm_end++;
            char *lbl = tm_end;
            if (*tm_end != '\0') {
                *tm_end = '\0';
                lbl++;
            }
            while (*lbl == ' ') lbl++;
            char *lbl_end = lbl;
            while (*lbl_end && *lbl_end != ' ') lbl_end++;
            *lbl_end = '\0';
            long long tm = atoll(tm_s);
            for (size_t i = 0; i < count; ) {
                if (list[i].time == tm && strcmp(list[i].label, lbl) == 0) {
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
            char *lbl = p;
            while (*lbl == ' ') lbl++;
            char *lbl_end = lbl;
            while (*lbl_end && *lbl_end != ' ') lbl_end++;
            *lbl_end = '\0';
            int found_idx = -1;
            long long min_tm = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].label, lbl) == 0) {
                    if (found_idx == -1 || list[i].time < min_tm) {
                        min_tm = list[i].time;
                        found_idx = (int)i;
                    }
                }
            }
            if (found_idx != -1) {
                printf("%lld %s %s\n", list[found_idx].time, list[found_idx].label, list[found_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(list, count, sizeof(Activity), compare_activities);
            }
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
