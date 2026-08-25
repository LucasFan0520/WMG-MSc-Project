// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;
int nt = 0, ndo = 0, ndn = 0;

int find_task(Task *arr, int n, const char *t) {
    for (int i = 0; i < n; i++)
        if (strcmp(arr[i].title, t) == 0) return i;
    return -1;
}

void add_task(Task **arr, int *n, const char *t, const char *note) {
    *arr = realloc(*arr, sizeof(Task) * (*n + 1));
    (*arr)[*n].title = strdup(t);
    (*arr)[*n].note = strdup(note);
    (*n)++;
}

void move_task(Task **src, int *sn, Task **dst, int *dn, const char *t) {
    int i = find_task(*src, *sn, t);
    if (i != -1) {
        add_task(dst, dn, (*src)[i].title, (*src)[i].note);
        free((*src)[i].title);
        free((*src)[i].note);
        for (int j = i; j < *sn - 1; j++) (*src)[j] = (*src)[j + 1];
        (*sn)--;
    }
}

void remove_task(const char *t) {
    int i;
    if ((i = find_task(todo, nt, t)) != -1) {
        free(todo[i].title); free(todo[i].note);
        for (int j = i; j < nt - 1; j++) todo[j] = todo[j + 1];
        nt--;
    } else if ((i = find_task(doing, ndo, t)) != -1) {
        free(doing[i].title); free(doing[i].note);
        for (int j = i; j < ndo - 1; j++) doing[j] = doing[j + 1];
        ndo--;
    } else if ((i = find_task(done, ndn, t)) != -1) {
        free(done[i].title); free(done[i].note);
        for (int j = i; j < ndn - 1; j++) done[j] = done[j + 1];
        ndn--;
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "TODO") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *p = line + 5 + strlen(arg1);
            if (*p == ' ') p++;
            add_task(&todo, &nt, arg1, p);
        } else if (strcmp(cmd, "START") == 0) {
            sscanf(line, "%*s %s", arg1);
            move_task(&todo, &nt, &doing, &ndo, arg1);
        } else if (strcmp(cmd, "DONE") == 0) {
            sscanf(line, "%*s %s", arg1);
            move_task(&doing, &ndo, &done, &ndn, arg1);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s", arg1);
            remove_task(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i;
            if ((i = find_task(todo, nt, arg1)) != -1) printf("todo %s\n", todo[i].note);
            else if ((i = find_task(doing, ndo, arg1)) != -1) printf("doing %s\n", doing[i].note);
            else if ((i = find_task(done, ndn, arg1)) != -1) printf("done %s\n", done[i].note);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < nt; i++) printf("todo %s %s\n", todo[i].title, todo[i].note);
            for (int i = 0; i < ndo; i++) printf("doing %s %s\n", doing[i].title, doing[i].note);
            for (int i = 0; i < ndn; i++) printf("done %s %s\n", done[i].title, done[i].note);
        }
    }
    for (int i = 0; i < nt; i++) { free(todo[i].title); free(todo[i].note); } free(todo);
    for (int i = 0; i < ndo; i++) { free(doing[i].title); free(doing[i].note); } free(doing);
    for (int i = 0; i < ndn; i++) { free(done[i].title); free(done[i].note); } free(done);
    return 0;
}