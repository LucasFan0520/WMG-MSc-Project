// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    long long time;
    char *label;
    char *note;
} Activity;

int cmp_activity(const void *a, const void *b) {
    const Activity *pa = (const Activity *)a;
    const Activity *pb = (const Activity *)b;
    if (pa->time < pb->time) return -1;
    if (pa->time > pb->time) return 1;
    return strcmp(pa->label, pb->label);
}

int main(void) {
    Activity *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *time_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *note = p;
            size_t ntlen = strlen(note);
            while (ntlen > 0 && (note[ntlen-1] == '\n' || note[ntlen-1] == '\n')) {
                note[ntlen-1] = '\0';
                ntlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Activity *nitems = realloc(items, cap * sizeof(Activity));
                if (nitems) items = nitems;
            }
            items[count].time = atoll(time_s);
            items[count].label = mystrdup(label);
            items[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *label = p;
            size_t l_len = strlen(label);
            while (l_len > 0 && (label[l_len-1] == '\n' || label[l_len-1] == '\n')) {
                label[l_len-1] = '\0';
                l_len--;
            }
            long long tval = atoll(time_s);
            size_t i = 0;
            while (i < count) {
                if (items[i].time == tval && strcmp(items[i].label, label) == 0) {
                    free(items[i].label);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = p;
            size_t l_len = strlen(label);
            while (l_len > 0 && (label[l_len-1] == '\n' || label[l_len-1] == '\n')) {
                label[l_len-1] = '\0';
                l_len--;
            }
            int idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].label, label) == 0) {
                    if (idx == -1 || items[i].time < items[idx].time) {
                        idx = (int)i;
                    }
                }
            }
            if (idx != -1) {
                printf("%lld %s %s\n", items[idx].time, items[idx].label, items[idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 1) {
                qsort(items, count, sizeof(Activity), cmp_activity);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%lld %s %s\n", items[i].time, items[i].label, items[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(items[i].label);
                free(items[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].label);
        free(items[i].note);
    }
    free(items);
    return 0;
}
