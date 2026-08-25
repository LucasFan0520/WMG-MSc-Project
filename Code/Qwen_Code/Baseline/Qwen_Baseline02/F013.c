// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *note; } Task;
Task todo[1000], doing[1000], done[1000];
int nt = 0, nd = 0, ndn = 0;

int find(Task *arr, int n, char *title) {
    for (int i = 0; i < n; i++) if (!strcmp(arr[i].title, title)) return i;
    return -1;
}

int main() {
    char cmd[20], title[100], note[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "TODO")) {
            scanf("%s", title); fgets(note, sizeof(note), stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            todo[nt].title = strdup(title); todo[nt++].note = strdup(note);
        } else if (!strcmp(cmd, "START")) {
            scanf("%s", title); int idx = find(todo, nt, title);
            if (idx != -1) {
                doing[nd] = todo[idx]; nd++;
                free(todo[idx].title);
                for (int i = idx; i < nt - 1; i++) todo[i] = todo[i + 1];
                nt--;
            }
        } else if (!strcmp(cmd, "DONE")) {
            scanf("%s", title); int idx = find(doing, nd, title);
            if (idx != -1) {
                done[ndn] = doing[idx]; ndn++;
                free(doing[idx].title);
                for (int i = idx; i < nd - 1; i++) doing[i] = doing[i + 1];
                nd--;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s", title);
            int idx = find(todo, nt, title);
            if (idx != -1) { free(todo[idx].title); free(todo[idx].note); for(int i=idx;i<nt-1;i++) todo[i]=todo[i+1]; nt--; continue; }
            idx = find(doing, nd, title);
            if (idx != -1) { free(doing[idx].title); free(doing[idx].note); for(int i=idx;i<nd-1;i++) doing[i]=doing[i+1]; nd--; continue; }
            idx = find(done, ndn, title);
            if (idx != -1) { free(done[idx].title); free(done[idx].note); for(int i=idx;i<ndn-1;i++) done[i]=done[i+1]; ndn--; }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", title);
            int idx = find(todo, nt, title); if(idx!=-1) { printf("todo %s\n", todo[idx].note); continue; }
            idx = find(doing, nd, title); if(idx!=-1) { printf("doing %s\n", doing[idx].note); continue; }
            idx = find(done, ndn, title); if(idx!=-1) printf("done %s\n", done[idx].note);
        } else if (!strcmp(cmd, "REPORT")) {
            printf("todo:"); for(int i=0;i<nt;i++) printf(" %s", todo[i].title); printf("\n");
            printf("doing:"); for(int i=0;i<nd;i++) printf(" %s", doing[i].title); printf("\n");
            printf("done:"); for(int i=0;i<ndn;i++) printf(" %s", done[i].title); printf("\n");
        }
    }
    for(int i=0;i<nt;i++){free(todo[i].title);free(todo[i].note);}
    for(int i=0;i<nd;i++){free(doing[i].title);free(doing[i].note);}
    for(int i=0;i<ndn;i++){free(done[i].title);free(done[i].note);}
    return 0;
}