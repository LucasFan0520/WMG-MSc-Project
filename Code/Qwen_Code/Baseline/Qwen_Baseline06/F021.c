// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *ing, *note; } Ing;
typedef struct { char *name; Ing *ings; int n_ing; } Rec;
Rec *recs; int n_rec;

int find_rec(char *name) {
    for(int i=0; i<n_rec; i++) if(!strcmp(recs[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], ing[100], note[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "RECIPE")) {
            scanf("%s", name);
            recs = realloc(recs, (n_rec+1)*sizeof(Rec));
            recs[n_rec].name = strdup(name);
            recs[n_rec].ings = NULL;
            recs[n_rec].n_ing = 0;
            n_rec++;
        } else if(!strcmp(cmd, "ING")) {
            scanf("%s %s", name, ing);
            fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            int i = find_rec(name);
            if(i != -1) {
                recs[i].ings = realloc(recs[i].ings, (recs[i].n_ing+1)*sizeof(Ing));
                recs[i].ings[recs[i].n_ing].ing = strdup(ing);
                recs[i].ings[recs[i].n_ing].note = strdup(note);
                recs[i].n_ing++;
            }
        } else if(!strcmp(cmd, "REMOVEING")) {
            scanf("%s %s", name, ing);
            int i = find_rec(name);
            if(i != -1) {
                for(int j=0; j<recs[i].n_ing; j++) {
                    if(!strcmp(recs[i].ings[j].ing, ing)) {
                        free(recs[i].ings[j].ing);
                        free(recs[i].ings[j].note);
                        for(int k=j; k<recs[i].n_ing-1; k++) recs[i].ings[k] = recs[i].ings[k+1];
                        recs[i].n_ing--;
                        break;
                    }
                }
            }
        } else if(!strcmp(cmd, "DELETERECIPE")) {
            scanf("%s", name);
            int i = find_rec(name);
            if(i != -1) {
                free(recs[i].name);
                for(int j=0; j<recs[i].n_ing; j++) {
                    free(recs[i].ings[j].ing);
                    free(recs[i].ings[j].note);
                }
                free(recs[i].ings);
                for(int k=i; k<n_rec-1; k++) recs[k] = recs[k+1];
                n_rec--;
            }
        } else if(!strcmp(cmd, "SHOW")) {
            scanf("%s", name);
            int i = find_rec(name);
            if(i != -1) {
                printf("%s\n", recs[i].name);
                for(int j=0; j<recs[i].n_ing; j++) {
                    printf("%s %s\n", recs[i].ings[j].ing, recs[i].ings[j].note);
                }
            }
        } else if(!strcmp(cmd, "LIST")) {
            for(int i=0; i<n_rec; i++) printf("%s\n", recs[i].name);
        }
    }
    return 0;
}