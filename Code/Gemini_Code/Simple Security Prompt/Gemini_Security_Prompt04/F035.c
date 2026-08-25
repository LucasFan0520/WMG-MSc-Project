// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *payload;
} Job;

int main(void) {
    Job *queue = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ENQUEUE ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *pay = p + 1;
                while (*pay == ' ') pay++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    queue = realloc(queue, cap * sizeof(Job));
                }
                queue[count].name = strdup(name);
                queue[count].payload = strdup(pay);
                count++;
            }
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *old = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *nw = p + 1;
                while (*nw == ' ') nw++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, old) == 0) {
                        if (count >= cap) {
                            cap = cap == 0 ? 16 : cap * 2;
                            queue = realloc(queue, cap * sizeof(Job));
                        }
                        queue[count].name = strdup(nw);
                        queue[count].payload = strdup(queue[i].payload);
                        count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(queue[found].name);
                free(queue[found].payload);
                for (int i = found; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "RUN ", 4) == 0) {
            char *name = line + 4;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s\n", queue[found].payload);
                free(queue[found].name);
                free(queue[found].payload);
                for (int i = found; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", queue[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].payload);
    }
    free(queue);
    return 0;
}
