// F027.c
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
    long priority;
    char *title;
    char *note;
    size_t ins_id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *aa = (const Reminder *)a;
    const Reminder *bb = (const Reminder *)b;
    if (aa->priority > bb->priority) return -1;
    if (aa->priority < bb->priority) return 1;
    if (aa->ins_id < bb->ins_id) return -1;
    if (aa->ins_id > bb->ins_id) return 1;
    return 0;
}

int main(void) {
    Reminder *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    size_t global_ins = 0;
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
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *pri_s = args;
            char *p2 = strchr(pri_s, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = p2 + 1;
                char *p3 = strchr(title, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    long pri = strtol(pri_s, NULL, 10);
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Reminder *narr = realloc(arr, ncap * sizeof(Reminder));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].priority = pri;
                        arr[count].title = strdup(title);
                        arr[count].note = strdup(note);
                        arr[count].ins_id = global_ins++;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                long delta = strtol(p2 + 1, NULL, 10);
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        arr[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            long n = strtol(args, NULL, 10);
            if (n > 0 && count > 0) {
                Reminder *temp = malloc(count * sizeof(Reminder));
                if (temp) {
                    memcpy(temp, arr, count * sizeof(Reminder));
                    qsort(temp, count, sizeof(Reminder), compare_reminders);
                    size_t limit = (size_t)n > count ? count : (size_t)n;
                    for (size_t i = 0; i < limit; i++) {
                        printf("%ld %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
                    }
                    free(temp);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            if (strcmp(args, "") == 0 || args != NULL) {
                for (size_t i = 0; i < count; i++) {
                    printf("%ld %s %s\n", arr[i].priority, arr[i].title, arr[i].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
