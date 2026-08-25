// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int time; char *label, *note; } A;
A *a; int n;

int cmp(const void *x, const void *y) {
    A *a1 = (A*)x, *a2 = (A*)y;
    if(a1->time != a2->time) return a1->time - a2->time;
    return strcmp(a1->label, a2->label);
}

int main() {
    char c[20], label[100], note[1000]; int time;
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "ADD")) {
            scanf("%d %s", &time, label); fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            a = realloc(a, (n+1)*sizeof(A));
            a[n].time=time; a[n].label=strdup(label); a[n].note=strdup(note); n++;
        } else if(!strcmp(c, "DELETE")) {
            scanf("%d %s", &time, label);
            for(int i=0; i<n; ) {
                if(a[i].time==time && !strcmp(a[i].label, label)) {
                    free(a[i].label); free(a[i].note);
                    for(int j=i; j<n-1; j++) a[j]=a[j+1];
                    n--;
                } else i++;
            }
        } else if(!strcmp(c, "FIND")) {
            scanf("%s", label);
            int min_t = 1e9, idx = -1;
            for(int i=0; i<n; i++) {
                if(!strcmp(a[i].label, label) && a[i].time < min_t) {
                    min_t = a[i].time; idx = i;
                }
            }
            if(idx!=-1) printf("%d %s %s\n", a[idx].time, a[idx].label, a[idx].note);
        } else if(!strcmp(c, "SORT")) {
            qsort(a, n, sizeof(A), cmp);
            for(int i=0; i<n; i++) printf("%d %s %s\n", a[i].time, a[i].label, a[i].note);
        } else if(!strcmp(c, "CLEAR")) {
            for(int i=0; i<n; i++) { free(a[i].label); free(a[i].note); }
            n = 0;
        }
    }
    return 0;
}