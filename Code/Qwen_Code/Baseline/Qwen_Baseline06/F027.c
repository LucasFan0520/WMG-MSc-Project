// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int pri; char *title, *note; } Rem;
Rem *rems; int n_rem;

int find_rem(char *title) {
    for(int i=0; i<n_rem; i++) if(!strcmp(rems[i].title, title)) return i;
    return -1;
}

int cmp(const void *a, const void *b) {
    return ((Rem*)b)->pri - ((Rem*)a)->pri;
}

int main() {
    char cmd[20], title[100], note[1000]; int pri, delta, n;
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "ADD")) {
            scanf("%d %s", &pri, title);
            fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            rems = realloc(rems, (n_rem+1)*sizeof(Rem));
            rems[n_rem].pri = pri;
            rems[n_rem].title = strdup(title);
            rems[n_rem].note = strdup(note);
            n_rem++;
        } else if(!strcmp(cmd, "BUMP")) {
            scanf("%s %d", title, &delta);
            int i = find_rem(title);
            if(i != -1) rems[i].pri += delta;
        } else if(!strcmp(cmd, "REMOVE")) {
            scanf("%s", title);
            int i = find_rem(title);
            if(i != -1) {
                free(rems[i].title);
                free(rems[i].note);
                for(int k=i; k<n_rem-1; k++) rems[k] = rems[k+1];
                n_rem--;
            }
        } else if(!strcmp(cmd, "TOP")) {
            scanf("%d", &n);
            Rem *tmp = malloc(n_rem * sizeof(Rem));
            memcpy(tmp, rems, n_rem * sizeof(Rem));
            qsort(tmp, n_rem, sizeof(Rem), cmp);
            int limit = n < n_rem ? n : n_rem;
            for(int i=0; i<limit; i++) {
                printf("%d %s %s\n", tmp[i].pri, tmp[i].title, tmp[i].note);
            }
            free(tmp);
        } else if(!strcmp(cmd, "LIST")) {
            for(int i=0; i<n_rem; i++) {
                printf("%d %s %s\n", rems[i].pri, rems[i].title, rems[i].note);
            }
        }
    }
    return 0;
}