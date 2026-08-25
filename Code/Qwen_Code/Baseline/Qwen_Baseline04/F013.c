// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;
int tcount = 0, docount = 0, dncount = 0;

int find_task(Task *list, int count, const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].title, title) == 0) return i;
    }
    return -1;
}

void move_task(Task **from, int *fc, Task **to, int *tc, const char *title) {
    int idx = find_task(*from, *fc, title);
    if (idx != -1) {
        *to = realloc(*to, sizeof(Task) * (*tc + 1));
        (*to)[*tc] = (*from)[idx];
        (*tc)++;
        for (int i = idx; i < *fc - 1; i++) (*from)[i] = (*from)[i+1];
        (*fc)--;
    }
}

void remove_task(Task **list, int *count, const char *title) {
    int idx = find_task(*list, *count, title);
    if (idx != -1) {
        free((*list)[idx].title);
        free((*list)[idx].note);
        for (int i = idx; i < *count - 1; i++) (*list)[i] = (*list)[i+1];
        (*count)--;
    }
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "TODO") == 0) {
            char title[100], note[1000];
            scanf("%s", title);
            scanf(" %[^\n]", note);
            todo = realloc(todo, sizeof(Task) * (tcount + 1));
            todo[tcount].title = strdup(title);
            todo[tcount].note = strdup(note);
            tcount++;
        } else if (strcmp(cmd, "START") == 0) {
            char title[100];
            scanf("%s", title);
            move_task(&todo, &tcount, &doing, &docount, title);
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[100];
            scanf("%s", title);
            move_task(&doing, &docount, &done, &dncount, title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[100];
            scanf("%s", title);
            remove_task(&todo, &tcount, title);
            remove_task(&doing, &docount, title);
            remove_task(&done, &dncount, title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[100];
            scanf("%s", title);
            int idx;
            if ((idx = find_task(todo, tcount, title)) != -1) printf("todo %s\n", todo[idx].note);
            else if ((idx = find_task(doing, docount, title)) != -1) printf("doing %s\n", doing[idx].note);
            else if ((idx = find_task(done, dncount, title)) != -1) printf("done %s\n", done[idx].note);
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("todo:");
            for (int i = 0; i < tcount; i++) printf(" %s", todo[i].title);
            printf("\ndoing:");
            for (int i = 0; i < docount; i++) printf(" %s", doing[i].title);
            printf("\ndone:");
            for (int i = 0; i < dncount; i++) printf(" %s", done[i].title);
            printf("\n");
        }
    }
    for (int i = 0; i < tcount; i++) { free(todo[i].title); free(todo[i].note); }
    for (int i = 0; i < docount; i++) { free(doing[i].title); free(doing[i].note); }
    for (int i = 0; i < dncount; i++) { free(done[i].title); free(done[i].note); }
    free(todo); free(doing); free(done);
    return 0;
}