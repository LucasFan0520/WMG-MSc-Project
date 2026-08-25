/* F013.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    int list; // 0=todo, 1=doing, 2=done
} Task;

static Task *tasks = NULL;
static size_t tcount = 0, tcap = 0;

static Task *find_task(const char *title) {
    for (size_t i = 0; i < tcount; i++) {
        if (strcmp(tasks[i].title, title) == 0)
            return &tasks[i];
    }
    return NULL;
}

static void add_task(const char *title, const char *note, int list) {
    if (tcount == tcap) {
        tcap = tcap ? tcap * 2 : 4;
        tasks = realloc(tasks, tcap * sizeof(Task));
        if (!tasks) { perror("realloc"); exit(1); }
    }
    tasks[tcount].title = strdup(title);
    tasks[tcount].note = strdup(note);
    tasks[tcount].list = list;
    tcount++;
}

static void move_task(const char *title, int newlist) {
    Task *t = find_task(title);
    if (t) t->list = newlist;
}

static void remove_task(const char *title) {
    for (size_t i = 0; i < tcount; i++) {
        if (strcmp(tasks[i].title, title) == 0) {
            free(tasks[i].title);
            free(tasks[i].note);
            for (size_t j = i; j < tcount - 1; j++)
                tasks[j] = tasks[j + 1];
            tcount--;
            return;
        }
    }
}

static void find_task_list(const char *title) {
    Task *t = find_task(title);
    if (!t) return;
    const char *listnames[] = {"todo", "doing", "done"};
    printf("%s %s\n", listnames[t->list], t->note);
}

static void report_tasks(void) {
    const char *listnames[] = {"todo", "doing", "done"};
    for (int l = 0; l < 3; l++) {
        printf("%s\n", listnames[l]);
        for (size_t i = 0; i < tcount; i++) {
            if (tasks[i].list == l) {
                printf("  %s %s\n", tasks[i].title, tasks[i].note);
            }
        }
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            if (find_task(title)) {
                // update note? Spec says "TODO title note adds a task" - if exists? Not specified. We'll replace? Or ignore? We'll treat as add, but if exists we update note? I'll set list to todo.
                Task *t = find_task(title);
                free(t->note);
                t->note = strdup(note);
                t->list = 0;
            } else {
                add_task(title, note, 0);
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title = strtok(NULL, " ");
            if (title) move_task(title, 1);
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) move_task(title, 2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_task_list(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tasks();
        }
    }
    for (size_t i = 0; i < tcount; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
    }
    free(tasks);
    return 0;
}