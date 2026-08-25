// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } Set;
Set *sets; int n_set;
Set *tsets; int n_tset;
int in_trans = 0;

int find(Set *arr, int sz, char *key) {
    for(int i=0; i<sz; i++) if(!strcmp(arr[i].key, key)) return i;
    return -1;
}

int main() {
    char cmd[20], key[100], val[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "SET")) {
            scanf("%s", key);
            fgets(val, 1000, stdin);
            while(val[0]==' ') memmove(val, val+1, strlen(val));
            val[strcspn(val, "\n")] = 0;
            int idx = find(sets, n_set, key);
            if(idx != -1) { free(sets[idx].val); sets[idx].val = strdup(val); }
            else { sets = realloc(sets, (n_set+1)*sizeof(Set)); sets[n_set].key=strdup(key); sets[n_set].val=strdup(val); n_set++; }
        } else if(!strcmp(cmd, "BEGIN")) {
            in_trans = 1;
            n_tset = 0;
        } else if(!strcmp(cmd, "TSET")) {
            scanf("%s", key);
            fgets(val, 1000, stdin);
            while(val[0]==' ') memmove(val, val+1, strlen(val));
            val[strcspn(val, "\n")] = 0;
            tsets = realloc(tsets, (n_tset+1)*sizeof(Set));
            tsets[n_tset].key = strdup(key);
            tsets[n_tset].val = strdup(val);
            n_tset++;
        } else if(!strcmp(cmd, "COMMIT")) {
            for(int i=0; i<n_tset; i++) {
                int idx = find(sets, n_set, tsets[i].key);
                if(idx != -1) { free(sets[idx].val); sets[idx].val = tsets[i].val; }
                else { sets = realloc(sets, (n_set+1)*sizeof(Set)); sets[n_set]=tsets[i]; n_set++; }
                free(tsets[i].key);
            }
            n_tset = 0;
            in_trans = 0;
        } else if(!strcmp(cmd, "ABORT")) {
            for(int i=0; i<n_tset; i++) { free(tsets[i].key); free(tsets[i].val); }
            n_tset = 0;
            in_trans = 0;
        } else if(!strcmp(cmd, "GET")) {
            scanf("%s", key);
            int idx = find(sets, n_set, key);
            if(idx != -1) printf("%s\n", sets[idx].val);
        } else if(!strcmp(cmd, "DUMP")) {
            for(int i=0; i<n_set; i++) printf("%s %s\n", sets[i].key, sets[i].val);
        }
    }
    return 0;
}