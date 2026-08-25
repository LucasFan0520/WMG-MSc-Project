// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines; int n;

int main() {
    char c[20], txt[1000]; int idx;
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "INSERT")) {
            scanf("%d", &idx); fgets(txt, 1000, stdin);
            while(txt[0]==' ') memmove(txt, txt+1, strlen(txt));
            txt[strcspn(txt, "\n")] = 0;
            if(idx>=0 && idx<=n) {
                lines = realloc(lines, (n+1)*sizeof(char*));
                for(int i=n; i>idx; i--) lines[i]=lines[i-1];
                lines[idx] = strdup(txt); n++;
            }
        } else if(!strcmp(c, "APPEND")) {
            fgets(txt, 1000, stdin);
            while(txt[0]==' ') memmove(txt, txt+1, strlen(txt));
            txt[strcspn(txt, "\n")] = 0;
            lines = realloc(lines, (n+1)*sizeof(char*));
            lines[n++] = strdup(txt);
        } else if(!strcmp(c, "DELETE")) {
            scanf("%d", &idx);
            if(idx>=0 && idx<n) {
                free(lines[idx]);
                for(int i=idx; i<n-1; i++) lines[i]=lines[i+1];
                n--;
            }
        } else if(!strcmp(c, "PATCH")) {
            scanf("%d", &idx); fgets(txt, 1000, stdin);
            while(txt[0]==' ') memmove(txt, txt+1, strlen(txt));
            txt[strcspn(txt, "\n")] = 0;
            if(idx>=0 && idx<n) { free(lines[idx]); lines[idx]=strdup(txt); }
        } else if(!strcmp(c, "PRINT")) {
            for(int i=0; i<n; i++) {
                for(char *p=lines[i]; *p; p++) putchar(*p==' '?'_':*p);
                putchar('\n');
            }
        }
    }
    return 0;
}