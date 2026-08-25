// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; int score; } S;
S *a; int n;

int find(char *nm) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].name, nm)) return i;
    return -1;
}

int main() {
    char c[20], nm[100], note[1000]; int score;
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "ADD")) {
            scanf("%s %d", nm, &score); fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            a = realloc(a, (n+1)*sizeof(S));
            a[n].name=strdup(nm); a[n].score=score; a[n].note=strdup(note); n++;
        } else if(!strcmp(c, "UPDATE")) {
            scanf("%s %d", nm, &score); fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            int x = find(nm);
            if(x!=-1) { free(a[x].note); a[x].score=score; a[x].note=strdup(note); }
        } else if(!strcmp(c, "DROP")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) {
                free(a[x].name); free(a[x].note);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "AVERAGE")) {
            int sum=0;
            for(int i=0; i<n; i++) sum += a[i].score;
            printf("%d\n", n>0 ? sum/n : 0);
        } else if(!strcmp(c, "LIST")) {
            for(int i=0; i<n; i++) printf("%s %d %s\n", a[i].name, a[i].score, a[i].note);
        }
    }
    return 0;
}