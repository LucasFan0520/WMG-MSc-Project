// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *email, *phone, *note; } C;
C *a; int n;

int find(char *nm) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].name, nm)) return i;
    return -1;
}

int main() {
    char c[20], nm[100], em[100], ph[100], note[1000], keep[100], rem[100];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "ADD")) {
            scanf("%s %s %s", nm, em, ph);
            a = realloc(a, (n+1)*sizeof(C));
            a[n].name=strdup(nm); a[n].email=strdup(em); a[n].phone=strdup(ph); a[n].note=strdup(""); n++;
        } else if(!strcmp(c, "NOTE")) {
            scanf("%s", nm); fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            int x = find(nm);
            if(x!=-1) { free(a[x].note); a[x].note=strdup(note); }
        } else if(!strcmp(c, "MERGE")) {
            scanf("%s %s", keep, rem);
            int k = find(keep), r = find(rem);
            if(k!=-1 && r!=-1) {
                char *nn = malloc(strlen(a[k].note)+strlen(a[r].note)+2);
                sprintf(nn, "%s %s", a[k].note, a[r].note);
                free(a[k].note); a[k].note=nn;
                free(a[r].name); free(a[r].email); free(a[r].phone); free(a[r].note);
                for(int i=r; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "DELETE")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) {
                free(a[x].name); free(a[x].email); free(a[x].phone); free(a[x].note);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "FIND")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) printf("%s %s %s %s\n", a[x].name, a[x].email, a[x].phone, a[x].note);
        } else if(!strcmp(c, "LIST")) {
            for(int i=0; i<n; i++) printf("%s %s %s %s\n", a[i].name, a[i].email, a[i].phone, a[i].note);
        }
    }
    return 0;
}