/* F007.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

static Activity *acts = NULL;
static size_t acount = 0, acap = 0;

static void add_activity(const char *time, const char *label, const char *note) {
    if (acount == acap) {
        acap = acap ? acap * 2 : 4;
        acts = realloc(acts, acap * sizeof(Activity));
        if (!acts) { perror("realloc"); exit(1); }
    }
    acts[acount].time = strdup(time);
    acts[acount].label = strdup(label);
    acts[acount].note = strdup(note);
    acount++;
}

static void delete_activities(const char *time, const char *label) {
    for (size_t i = 0; i < acount; ) {
        if (strcmp(acts[i].time, time) == 0 && strcmp(acts[i].label, label) == 0) {
            free(acts[i].time);
            free(acts[i].label);
            free(acts[i].note);
            for (size_t j = i; j < acount - 1; j++)
                acts[j] = acts[j + 1];
            acount--;
        } else {
            i++;
        }
    }
}

static void find_earliest(const char *label) {
    Activity *best = NULL;
    for (size_t i = 0; i < acount; i++) {
        if (strcmp(acts[i].label, label) == 0) {
            if (!best || strcmp(acts[i].time, best->time) < 0)
                best = &acts[i];
        }
    }
    if (best) {
        printf("%s %s %s\n", best->time, best->label, best->note);
    }
}

static int cmp_act(const void *a, const void *b) {
    const Activity *aa = (const Activity *)a;
    const Activity *bb = (const Activity *)b;
    int c = strcmp(aa->time, bb->time);
    if (c) return c;
    return strcmp(aa->label, bb->label);
}

static void sort_activities(void) {
    qsort(acts, acount, sizeof(Activity), cmp_act);
    for (size_t i = 0; i < acount; i++) {
        printf("%s %s %s\n", acts[i].time, acts[i].label, acts[i].note);
    }
}

static void clear_activities(void) {
    for (size_t i = 0; i < acount; i++) {
        free(acts[i].time);
        free(acts[i].label);
        free(acts[i].note);
    }
    acount = 0;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *time = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (!time || !label) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (time && label) delete_activities(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = strtok(NULL, " ");
            if (label) find_earliest(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_activities();
        }
    }
    for (size_t i = 0; i < acount; i++) {
        free(acts[i].time);
        free(acts[i].label);
        free(acts[i].note);
    }
    free(acts);
    return 0;
}