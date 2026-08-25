// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char **vals; int vc; } Idx;
Idx *idxs = NULL; int ic = 0;

int find_key(const char *k) {
    for (int i = 0; i < ic; i++) if (strcmp(idxs[i].key, k) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[256];
        c2[0] = c3[0] = 0;
        sscanf(line, "%19s %255s %255[^\n]", c1, c2, c3);
        
        if (strcmp(c1, "ADD") == 0) {
            int i = find_key(c2);
            if (i < 0) { idxs = realloc(idxs, (ic+1)*sizeof(Idx)); idxs[ic].key = strdup(c2); idxs[ic].vals = NULL; idxs[ic].vc = 0; i = ic++; }
            idxs[i].vals = realloc(idxs[i].vals, (idxs[i].vc+1)*sizeof(char*));
            idxs[i].vals[idxs[i].vc++] = strdup(c3);
        } else if (strcmp(c1, "REMOVE") == 0) {
            int i = find_key(c2);
            if (i >= 0) {
                for (int j = 0; j < idxs[i].vc; j++) {
                    if (strcmp(idxs[i].vals[j], c3) == 0) {
                        free(idxs[i].vals[j]);
                        for (int k = j; k < idxs[i].vc-1; k++) idxs[i].vals[k] = idxs[i].vals[k+1];
                        idxs[i].vc--; break;
                    }
                }
            }
        } else if (strcmp(c1, "ERASE") == 0) {
            int i = find_key(c2);
            if (i >= 0) {
                free(idxs[i].key);
                for (int j = 0; j < idxs[i].vc; j++) free(idxs[i].vals[j]);
                free(idxs[i].vals);
                for (int j = i; j < ic-1; j++) idxs[j] = idxs[j+1];
                ic--;
            }
        } else if (strcmp(c1, "VALUES") == 0) {
            int i = find_key(c2);
            if (i >= 0) for (int j = 0; j < idxs[i].vc; j++) printf("%s\n", idxs[i].vals[j]);
        } else if (strcmp(c1, "KEYS") == 0) {
            for (int i = 0; i < ic; i++) printf("%s\n", idxs[i].key);
        }
    }
    for (int i = 0; i < ic; i++) {
        free(idxs[i].key);
        for (int j = 0; j < idxs[i].vc; j++) free(idxs[i].vals[j]);
        free(idxs[i].vals);
    }
    free(idxs);
    return 0;
}