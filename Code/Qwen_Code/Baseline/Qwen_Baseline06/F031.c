// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; } Rec;
Rec *recs; int n_rec;

int find_rec(char *name) {
    for(int i=0; i<n_rec; i++) if(!strcmp(recs[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], note[1000]; int n;
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "BATCH")) {
            scanf("%d", &n);
            Rec *tmp = malloc(n * sizeof(Rec));
            int valid = 1;
            for(int i=0; i<n; i++) {
                if(scanf("%s", tmp[i].name) != 1) { valid = 0; break; }
                fgets(note, 1000, stdin);
                while(note[0]==' ') memmove(note, note+1, strlen(note));
                note[strcspn(note, "\n")] = 0;
                tmp[i].note = strdup(note);
            }
            if(valid) {
                recs = realloc(recs, (n_rec+n)*sizeof(Rec));
                for(int i=0; i<n; i++) recs[n_rec++] = tmp[i];
            } else {
                for(int i=0; i<n; i++) free(tmp[i].note);
            }
            free(tmp);
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_rec(name);
            if(idx != -1) {
                free(recs[idx].name);
                free(recs[idx].note);
                for(int k=idx; k<n_rec-1; k++) recs[k] = recs[k+1];
                n_rec--;
            }
        } else if(!strcmp(cmd, "FIND")) {
            scanf("%s", name);
            int idx = find_rec(name);
            if(idx != -1) printf("%s %s\n", recs[idx].name, recs[idx].note);
        } else if(!strcmp(cmd, "REPORT")) {
            for(int i=0; i<n_rec; i++) {
                printf("%s %s\n", recs[i].name, recs[i].note);
            }
        }
    }
    return 0;
}