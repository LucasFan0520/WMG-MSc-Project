// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *note; } Task;
Task *todo=0, *doing=0, *done=0;
int nt=0, ct=0, nd=0, cd=0, nD=0, cD=0;

void add(Task **arr, int *n, int *cap, char *title, char *note) {
    if (*n == *cap) { *cap = *cap ? *cap*2 : 4; *arr = realloc(*arr, *cap*sizeof(Task)); }
    (*arr)[*n].title = strdup(title); (*arr)[*n].note = strdup(note); (*n)++;
}

int find(Task *arr, int n, char *title) {
    for (int i=0; i<n; i++) if (!strcmp(arr[i].title, title)) return i;
    return -1;
}

void move(Task **from, int *fn, Task **to, int *tn, int *tc, char *title) {
    int i = find(*from, *fn, title);
    if (i >= 0) {
        add(to, tn, tc, (*from)[i].title, (*from)[i].note);
        free((*from)[i].title); free((*from)[i].note);
        for (int j=i; j<*fn-1; j++) (*from)[j] = (*from)[j+1]; (*fn)--;
    }
}

void remove_task(Task **arr, int *n, char *title) {
    int i = find(*arr, *n, title);
    if (i >= 0) {
        free((*arr)[i].title); free((*arr)[i].note);
        for (int j=i; j<*n-1; j++) (*arr)[j] = (*arr)[j+1]; (*n)--;
    }
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], title[256], rest[1700];
        rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "REMOVE") && strcmp(cmd, "REPORT")) sscanf(line, "%*s %s %[^\n]", title, rest);
        else if (!strcmp(cmd, "REMOVE")) sscanf(line, "%*s %s", title);

        if (!strcmp(cmd, "TODO")) add(&todo, &nt, &ct, title, rest);
        else if (!strcmp(cmd, "START")) move(&todo, &nt, &doing, &nd, &cd, title);
        else if (!strcmp(cmd, "DONE")) move(&doing, &nd, &done, &nD, &cD, title);
        else if (!strcmp(cmd, "REMOVE")) {
            remove_task(&todo, &nt, title);
            remove_task(&doing, &nd, title);
            remove_task(&done, &nD, title);
        } else if (!strcmp(cmd, "FIND")) {
            int i;
            if ((i = find(todo, nt, title)) >= 0) printf("todo %s\n", todo[i].note);
            else if ((i = find(doing, nd, title)) >= 0) printf("doing %s\n", doing[i].note);
            else if ((i = find(done, nD, title)) >= 0) printf("done %s\n", done[i].note);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i=0; i<nt; i++) printf("todo %s %s\n", todo[i].title, todo[i].note);
            for (int i=0; i<nd; i++) printf("doing %s %s\n", doing[i].title, doing[i].note);
            for (int i=0; i<nD; i++) printf("done %s %s\n", done[i].title, done[i].note);
        }
    }
    return 0;
}