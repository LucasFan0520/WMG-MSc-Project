// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *label; } L;
typedef struct { char *name, *note; L *labels; int count; } I;
I *a; int n;

int find(char *nm) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].name, nm)) return i;
    return -1;
}

int main() {
    char c[20], nm[100], note[1000], lb[100];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "ITEM")) {
            scanf("%s", nm); fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            a = realloc(a, (n+1)*sizeof(I));
            a[n].name=strdup(nm); a[n].note=strdup(note); a[n].labels=NULL; a[n].count=0; n++;
        } else if(!strcmp(c, "LABEL")) {
            scanf("%s %s", nm, lb); int x = find(nm);
            if(x!=-1) {
                a[x].labels = realloc(a[x].labels, (a[x].count+1)*sizeof(L));
                a[x].labels[a[x].count++].label = strdup(lb);
            }
        } else if(!strcmp(c, "UNLABEL")) {
            scanf("%s %s", nm, lb); int x = find(nm);
            if(x!=-1) {
                for(int i=0; i<a[x].count; i++) {
                    if(!strcmp(a[x].labels[i].label, lb)) {
                        free(a[x].labels[i].label);
                        for(int j=i; j<a[x].count-1; j++) a[x].labels[j]=a[x].labels[j+1];
                        a[x].count--; break;
                    }
                }
            }
        } else if(!strcmp(c, "DELETE")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) {
                free(a[x].name); free(a[x].note);
                for(int i=0; i<a[x].count; i++) free(a[x].labels[i].label);
                free(a[x].labels);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "FIND")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) {
                printf("%s %s", a[x].name, a[x].note);
                for(int i=0; i<a[x].count; i++) printf(" %s", a[x].labels[i].label);
                printf("\n");
            }
        } else if(!strcmp(c, "REPORT")) {
            for(int i=0; i<n; i++) {
                printf("%s %s", a[i].name, a[i].note);
                for(int j=0; j<a[i].count; j++) printf(" %s", a[i].labels[j].label);
                printf("\n");
            }
        }
    }
    return 0;
}