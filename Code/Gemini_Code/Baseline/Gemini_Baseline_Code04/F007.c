// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int time;
    char *label;
    char *note;
} Activity;

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

int compare_activities(const void *a, const void *b) {
    Activity *actA = (Activity *)a;
    Activity *actB = (Activity *)b;
    if (actA->time != actB->time) {
        return actA->time - actB->time;
    }
    return strcmp(actA->label, actB->label);
}

int main() {
    Activity *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *time_s = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *label = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    arr = realloc(arr, (count + 1) * sizeof(Activity));
                    arr[count].time = atoi(time_s);
                    arr[count].label = strdup(label);
                    arr[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            char *time_s = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *label = p + 1;
                int t = atoi(time_s);
                int i = 0;
                while (i < count) {
                    if (arr[i].time == t && strcmp(arr[i].label, label) == 0) {
                        free(arr[i].label);
                        free(arr[i].note);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *label = line + 5;
            int earliest_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].label, label) == 0) {
                    if (earliest_idx == -1 || arr[i].time < arr[earliest_idx].time) {
                        earliest_idx = i;
                    }
                }
            }
            if (earliest_idx != -1) {
                printf("%d %s %s\n", arr[earliest_idx].time, arr[earliest_idx].label, arr[earliest_idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            Activity *temp = malloc(count * sizeof(Activity));
            memcpy(temp, arr, count * sizeof(Activity));
            qsort(temp, count, sizeof(Activity), compare_activities);
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", temp[i].time, temp[i].label, temp[i].note);
            }
            free(temp);
        } else if (strcmp(line, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(arr[i].label);
                free(arr[i].note);
            }
            free(arr);
            arr = NULL;
            count = 0;
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].label);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
