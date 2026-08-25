// F027.c
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
    int priority;
    char *title;
    char *note;
    size_t id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *aa = (const Reminder *)a;
    const Reminder *bb = (const Reminder *)b;
    if (aa->priority > bb->priority) return -1;
    if (aa->priority < bb->priority) return 1;
    if (aa->id < bb->id) return -1;
    return 1;
}

int main(void) {
    Reminder *rems = NULL;
    size_t count = 0;
    size_t capacity = 0;
    size_t next_id = 0;
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
            char *pri_s = p;
            while (*pri_s == ' ') pri_s++;
            char *pri_end = pri_s;
            while (*pri_end && *pri_end != ' ') pri_end++;
            char *title = pri_end;
            if (*pri_end != '\0') {
                *pri_end = '\0';
                title++;
            }
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            char *note = title_end;
            if (*title_end != '\0') {
                *title_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            int pri = atoi(pri_s);
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Reminder *new_r = realloc(rems, capacity * sizeof(Reminder));
                if (new_r) rems = new_r;
            }
            rems[count].priority = pri;
            rems[count].title = mystrdup(title);
            rems[count].note = mystrdup(note);
            rems[count].id = next_id++;
            count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            char *delta_s = title_end;
            if (*title_end != '\0') {
                *title_end = '\0';
                delta_s++;
            }
            while (*delta_s == ' ') delta_s++;
            char *delta_end = delta_s;
            while (*delta_end && *delta_end != ' ') delta_end++;
            *delta_end = '\0';
            int delta = atoi(delta_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(rems[i].title, title) == 0) {
                    rems[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(rems[i].title, title) == 0) {
                    free(rems[i].title);
                    free(rems[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        rems[j] = rems[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n_s = p;
            while (*n_s == ' ') n_s++;
            char *n_end = n_s;
            while (*n_end && *n_end != ' ') n_end++;
            *n_end = '\0';
            int n = atoi(n_s);
            if (n < 0) n = 0;
            if (count > 0) {
                Reminder *temp = malloc(count * sizeof(Reminder));
                if (temp) {
                    memcpy(temp, rems, count * sizeof(Reminder));
                    qsort(temp, count, sizeof(Reminder), compare_reminders);
                    size_t lim = (size_t)n < count ? (size_t)n : count;
                    for (size_t i = 0; i < lim; i++) {
                        printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
                    }
                    free(temp);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%d %s %s\n", rems[i].priority, rems[i].title, rems[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(rems[i].title);
        free(rems[i].note);
    }
    free(rems);
    return 0;
}
