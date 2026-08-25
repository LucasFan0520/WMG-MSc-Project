// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLE;

RLE *rles = NULL;
int num_rles = 0;

int find_rle(const char *name) {
    for (int i = 0; i < num_rles; i++) {
        if (strcmp(rles[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "STORE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            char *enc = strstr(line, name);
            if (enc) {
                enc += strlen(name);
                if (*enc == ' ') enc++;
            } else {
                enc = "";
            }
            rles = realloc(rles, (num_rles + 1) * sizeof(RLE));
            rles[num_rles].name = strdup(name);
            rles[num_rles].encoded = strdup(enc);
            num_rles++;
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_rle(name);
            if (idx >= 0) {
                char *p = rles[idx].encoded;
                int valid = 1;
                while (*p) {
                    if (!isdigit(*p)) {
                        valid = 0;
                        break;
                    }
                    long count = strtol(p, &p, 10);
                    if (count > 1000000) {
                        valid = 0;
                        break;
                    }
                    if (!*p) {
                        valid = 0;
                        break;
                    }
                    char c = *p++;
                    for (long i = 0; i < count; i++) {
                        putchar(c == ' ' ? '_' : c);
                    }
                }
                if (!valid) {
                    printf("INVALID\n");
                } else {
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_rle(name);
            if (idx >= 0) {
                free(rles[idx].name);
                free(rles[idx].encoded);
                for (int i = idx; i < num_rles - 1; i++) {
                    rles[i] = rles[i + 1];
                }
                num_rles--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < num_rles; i++) {
                printf("%s\n", rles[i].name);
            }
        }
    }
    
    for (int i = 0; i < num_rles; i++) {
        free(rles[i].name);
        free(rles[i].encoded);
    }
    free(rles);
    
    return 0;
}