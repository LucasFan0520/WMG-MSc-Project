/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

static Person *queue = NULL;
static int qcount = 0;
static int qcap = 0;

static int ensure_cap(void) {
    if (qcount >= qcap) {
        int newcap = qcap == 0 ? 8 : qcap * 2;
        Person *tmp = realloc(queue, sizeof(Person) * newcap);
        if (!tmp) return -1;
        queue = tmp;
        qcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_person(const char *name) {
    for (int i = 0; i < qcount; i++) {
        if (strcmp(queue[i].name, name) == 0)
            return i;
    }
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "JOIN") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", name, &off2) != 1) continue;
            const char *note = line + offset + off2;
            while (*note == ' ') note++;
            if (ensure_cap() != 0) continue;
            queue[qcount].name = dup_str(name);
            queue[qcount].note = dup_str(note);
            if (!queue[qcount].name || !queue[qcount].note) {
                free(queue[qcount].name);
                free(queue[qcount].note);
                continue;
            }
            qcount++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (qcount == 0) continue;
            printf("%s %s\n", queue[0].name, queue[0].note);
            free(queue[0].name);
            free(queue[0].note);
            for (int i = 0; i < qcount - 1; i++)
                queue[i] = queue[i + 1];
            qcount--;
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_person(name);
            if (idx < 0) continue;
            free(queue[idx].name);
            free(queue[idx].note);
            for (int i = idx; i < qcount - 1; i++)
                queue[i] = queue[i + 1];
            qcount--;
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", name, &off2) != 1) continue;
            const char *note = line + offset + off2;
            while (*note == ' ') note++;
            if (ensure_cap() != 0) continue;
            queue[qcount].name = dup_str(name);
            queue[qcount].note = dup_str(note);
            if (!queue[qcount].name || !queue[qcount].note) {
                free(queue[qcount].name);
                free(queue[qcount].note);
                continue;
            }
            qcount++;
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_person(name);
            if (idx >= 0) {
                printf("%s\n", queue[idx].note);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = 0; i < qcount; i++) {
                printf("%s %s\n", queue[i].name, queue[i].note);
            }
        }
    }
    for (int i = 0; i < qcount; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
    return 0;
}