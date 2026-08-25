// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *note; } Task;
Task *todo = NULL, *doing = NULL, *done = NULL;
int tc = 0, dc = 0, dnc = 0;

int find_task(Task *arr, int cnt, const char *t) {
    for (int i = 0; i < cnt; i++) if (strcmp(arr[i].title, t) == 0) return i;
    return -1;
}

void move_task(Task **src, int *sc, Task **dst, int *dc, const char *t) {
    int i = find_task(*src, *sc, t);
    if (i >= 0) {
        *dst = realloc(*dst, (*dc+1)*sizeof(Task));
        (*dst)[*dc] = (*src)[i];
        (*dc)++;
        for (int j = i; j < *sc-1; j++) (*src)[j] = (*src)[j+1];
        (*sc)--;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[700];
        c2[0] = c3[0] = 0;
        sscanf(line, "%19s %255s %700[^\n]", c1, c2, c3);
        
        if (strcmp(c1, "TODO") == 0) {
            todo = realloc(todo, (tc+1)*sizeof(Task));
            todo[tc].title = strdup(c2); todo[tc].note = strdup(c3); tc++;
        } else if (strcmp(c1, "START") == 0) {
            move_task(&todo, &tc, &doing, &dc, c2);
        } else if (strcmp(c1, "DONE") == 0) {
            move_task(&doing, &dc, &done, &dnc, c2);
        } else if (strcmp(c1, "REMOVE") == 0) {
            int i;
            if ((i = find_task(todo, tc, c2)) >= 0) { free(todo[i].title); free(todo[i].note); for(int j=i; j<tc-1; j++) todo[j]=todo[j+1]; tc--; }
            else if ((i = find_task(doing, dc, c2)) >= 0) { free(doing[i].title); free(doing[i].note); for(int j=i; j<dc-1; j++) doing[j]=doing[j+1]; dc--; }
            else if ((i = find_task(done, dnc, c2)) >= 0) { free(done[i].title); free(done[i].note); for(int j=i; j<dnc-1; j++) done[j]=done[j+1]; dnc--; }
        } else if (strcmp(c1, "FIND") == 0) {
            int i;
            if ((i = find_task(todo, tc, c2)) >= 0) printf("todo %s\n", todo[i].note);
            else if ((i = find_task(doing, dc, c2)) >= 0) printf("doing %s\n", doing[i].note);
            else if ((i = find_task(done, dnc, c2)) >= 0) printf("done %s\n", done[i].note);
        } else if (strcmp(c1, "REPORT") == 0) {
            printf("todo:\n"); for (int i = 0; i < tc; i++) printf("%s %s\n", todo[i].title, todo[i].note);
            printf("doing:\n"); for (int i = 0; i < dc; i++) printf("%s %s\n", doing[i].title, doing[i].note);
            printf("done:\n"); for (int i = 0; i < dnc; i++) printf("%s %s\n", done[i].title, done[i].note);
        }
    }
    for (int i = 0; i < tc; i++) { free(todo[i].title); free(todo[i].note); }
    for (int i = 0; i < dc; i++) { free(doing[i].title); free(doing[i].note); }
    for (int i = 0; i < dnc; i++) { free(done[i].title); free(done[i].note); }
    free(todo); free(doing); free(done);
    return 0;
}