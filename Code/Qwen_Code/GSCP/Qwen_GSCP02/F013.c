/* F013.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int list_id;
} Task;

static Task *tasks = NULL;
static int tcount = 0;
static int tcap = 0;

static int ensure_cap(void) {
    if (tcount >= tcap) {
        int newcap = tcap == 0 ? 8 : tcap * 2;
        Task *tmp = realloc(tasks, sizeof(Task) * newcap);
        if (!tmp) return -1;
        tasks = tmp;
        tcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_task(const char *title) {
    for (int i = 0; i < tcount; i++) {
        if (strcmp(tasks[i].title, title) == 0)
            return i;
    }
    return -1;
}

static const char *list_name(int id) {
    if (id == 0) return "todo";
    if (id == 1) return "doing";
    return "done";
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

        if (strcmp(cmd, "TODO") == 0) {
            char title[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", title, &off2) != 1) continue;
            const char *note = line + offset + off2;
            while (*note == ' ') note++;
            if (find_task(title) >= 0) continue;
            if (ensure_cap() != 0) continue;
            tasks[tcount].title = dup_str(title);
            tasks[tcount].note = dup_str(note);
            tasks[tcount].list_id = 0;
            if (!tasks[tcount].title || !tasks[tcount].note) {
                free(tasks[tcount].title);
                free(tasks[tcount].note);
                continue;
            }
            tcount++;
        } else if (strcmp(cmd, "START") == 0) {
            char title[256] = {0};
            if (sscanf(line + offset, " %255s", title) != 1) continue;
            int idx = find_task(title);
            if (idx < 0) continue;
            tasks[idx].list_id = 1;
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[256] = {0};
            if (sscanf(line + offset, " %255s", title) != 1) continue;
            int idx = find_task(title);
            if (idx < 0) continue;
            tasks[idx].list_id = 2;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256] = {0};
            if (sscanf(line + offset, " %255s", title) != 1) continue;
            int idx = find_task(title);
            if (idx < 0) continue;
            free(tasks[idx].title);
            free(tasks[idx].note);
            for (int i = idx; i < tcount - 1; i++)
                tasks[i] = tasks[i + 1];
            tcount--;
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256] = {0};
            if (sscanf(line + offset, " %255s", title) != 1) continue;
            int idx = find_task(title);
            if (idx >= 0) {
                printf("%s %s\n", list_name(tasks[idx].list_id), tasks[idx].note);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int l = 0; l < 3; l++) {
                printf("[%s]\n", list_name(l));
                for (int i = 0; i < tcount; i++) {
                    if (tasks[i].list_id == l) {
                        printf("  %s %s\n", tasks[i].title, tasks[i].note);
                    }
                }
            }
        }
    }
    for (int i = 0; i < tcount; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
    }
    free(tasks);
    return 0;
}