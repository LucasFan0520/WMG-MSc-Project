/* F007.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time_str;
    char *label;
    char *note;
} Activity;

static Activity *activities = NULL;
static int acount = 0;
static int acap = 0;

static int ensure_cap(void) {
    if (acount >= acap) {
        int newcap = acap == 0 ? 8 : acap * 2;
        Activity *tmp = realloc(activities, sizeof(Activity) * newcap);
        if (!tmp) return -1;
        activities = tmp;
        acap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int cmp_activity(const void *a, const void *b) {
    const Activity *aa = (const Activity *)a;
    const Activity *bb = (const Activity *)b;
    int c = strcmp(aa->time_str, bb->time_str);
    if (c != 0) return c;
    return strcmp(aa->label, bb->label);
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

        if (strcmp(cmd, "ADD") == 0) {
            char timestr[256] = {0}, label[256] = {0};
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, " %255s%n", timestr, &off2) != 1) continue;
            if (sscanf(line + offset + off2, " %255s%n", label, &off3) != 1) continue;
            const char *note = line + offset + off2 + off3;
            while (*note == ' ') note++;
            if (ensure_cap() != 0) continue;
            activities[acount].time_str = dup_str(timestr);
            activities[acount].label = dup_str(label);
            activities[acount].note = dup_str(note);
            if (!activities[acount].time_str || !activities[acount].label || !activities[acount].note) {
                free(activities[acount].time_str);
                free(activities[acount].label);
                free(activities[acount].note);
                continue;
            }
            acount++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char timestr[256] = {0}, label[256] = {0};
            if (sscanf(line + offset, " %255s %255s", timestr, label) != 2) continue;
            int i = 0;
            while (i < acount) {
                if (strcmp(activities[i].time_str, timestr) == 0 &&
                    strcmp(activities[i].label, label) == 0) {
                    free(activities[i].time_str);
                    free(activities[i].label);
                    free(activities[i].note);
                    for (int j = i; j < acount - 1; j++)
                        activities[j] = activities[j + 1];
                    acount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char label[256] = {0};
            if (sscanf(line + offset, " %255s", label) != 1) continue;
            int best = -1;
            for (int i = 0; i < acount; i++) {
                if (strcmp(activities[i].label, label) == 0) {
                    if (best < 0 || strcmp(activities[i].time_str, activities[best].time_str) < 0)
                        best = i;
                }
            }
            if (best >= 0) {
                printf("%s %s %s\n", activities[best].time_str, activities[best].label, activities[best].note);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(activities, acount, sizeof(Activity), cmp_activity);
            for (int i = 0; i < acount; i++) {
                printf("%s %s %s\n", activities[i].time_str, activities[i].label, activities[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < acount; i++) {
                free(activities[i].time_str);
                free(activities[i].label);
                free(activities[i].note);
            }
            acount = 0;
        }
    }
    for (int i = 0; i < acount; i++) {
        free(activities[i].time_str);
        free(activities[i].label);
        free(activities[i].note);
    }
    free(activities);
    return 0;
}