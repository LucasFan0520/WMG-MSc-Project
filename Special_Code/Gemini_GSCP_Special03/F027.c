// F027.c
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
    long long priority;
    char *title;
    char *note;
} Reminder;

int cmp_reminder(const void *a, const void *b) {
    const Reminder *pa = (const Reminder *)a;
    const Reminder *pb = (const Reminder *)b;
    if (pa->priority > pb->priority) return -1;
    if (pa->priority < pb->priority) return 1;
    return 0;
}

int main(void) {
    Reminder *items = NULL;
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
            char *prio_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *title = p;
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
                Reminder *nitems = realloc(items, cap * sizeof(Reminder));
                if (nitems) items = nitems;
            }
            items[count].priority = atoll(prio_s);
            items[count].title = mystrdup(title);
            items[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *delta_s = p;
            size_t dtlen = strlen(delta_s);
            while (dtlen > 0 && (delta_s[dtlen-1] == '\n' || delta_s[dtlen-1] == '\n')) {
                delta_s[dtlen-1] = '\0';
                dtlen--;
            }
            long long delta = atoll(delta_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    items[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = p;
            size_t ttlen = strlen(title);
            while (ttlen > 0 && (title[ttlen-1] == '\n' || title[ttlen-1] == '\n')) {
                title[ttlen-1] = '\0';
                ttlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n_s = p;
            size_t nlen = strlen(n_s);
            while (nlen > 0 && (n_s[nlen-1] == '\n' || n_s[nlen-1] == '\n')) {
                n_s[nlen-1] = '\0';
                nlen--;
            }
            int n = atoi(n_s);
            if (n > 0) {
                Reminder *temp = malloc(count * sizeof(Reminder));
                if (temp) {
                    for (size_t i = 0; i < count; i++) temp[i] = items[i];
                    if (count > 1) {
                        qsort(temp, count, sizeof(Reminder), cmp_reminder);
                    }
                    size_t limit = (size_t)n < count ? (size_t)n : count;
                    for (size_t i = 0; i < limit; i++) {
                        printf("%lld %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
                    }
                    free(temp);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%lld %s %s\n", items[i].priority, items[i].title, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].note);
    }
    free(items);
    return 0;
}
