/* F035.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

static Job *jobs = NULL;
static int jcount = 0;
static int jcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_job(const char *name) {
    for (int i = 0; i < jcount; i++)
        if (strcmp(jobs[i].name, name) == 0) return i;
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

        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", name, &off2) != 1) continue;
            const char *payload = line + offset + off2;
            while (*payload == ' ') payload++;
            if (jcount >= jcap) {
                jcap = jcap == 0 ? 8 : jcap * 2;
                jobs = realloc(jobs, sizeof(Job) * jcap);
            }
            jobs[jcount].name = dup_str(name);
            jobs[jcount].payload = dup_str(payload);
            jcount++;
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old_name[256] = {0}, new_name[256] = {0};
            if (sscanf(line + offset, "%255s %255s", old_name, new_name) != 2) continue;
            int idx = find_job(old_name);
            if (idx < 0) continue;
            if (jcount >= jcap) {
                jcap = jcap == 0 ? 8 : jcap * 2;
                jobs = realloc(jobs, sizeof(Job) * jcap);
            }
            jobs[jcount].name = dup_str(new_name);
            jobs[jcount].payload = dup_str(jobs[idx].payload);
            jcount++;
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_job(name);
            if (idx < 0) continue;
            free(jobs[idx].name);
            free(jobs[idx].payload);
            for (int i = idx; i < jcount - 1; i++) jobs[i] = jobs[i + 1];
            jcount--;
        } else if (strcmp(cmd, "RUN") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_job(name);
            if (idx < 0) continue;
            printf("%s %s\n", jobs[idx].name, jobs[idx].payload);
            free(jobs[idx].name);
            free(jobs[idx].payload);
            for (int i = idx; i < jcount - 1; i++) jobs[i] = jobs[i + 1];
            jcount--;
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < jcount; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }
    for (int i = 0; i < jcount; i++) {
        free(jobs[i].name);
        free(jobs[i].payload);
    }
    free(jobs);
    return 0;
}