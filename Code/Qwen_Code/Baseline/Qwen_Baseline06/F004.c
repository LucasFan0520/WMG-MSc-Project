// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } V;
V *a; int n;

int find(char *k) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].key, k)) return i;
    return -1;
}

int main() {
    char c[20], k[100], v[1000], txt[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "SET")) {
            scanf("%s", k); fgets(v, 1000, stdin);
            while(v[0]==' ') memmove(v, v+1, strlen(v));
            v[strcspn(v, "\n")] = 0;
            int x = find(k);
            if(x!=-1) { free(a[x].val); a[x].val = strdup(v); }
            else { a = realloc(a, (n+1)*sizeof(V)); a[n].key=strdup(k); a[n].val=strdup(v); n++; }
        } else if(!strcmp(c, "UNSET")) {
            scanf("%s", k); int x = find(k);
            if(x!=-1) {
                free(a[x].key); free(a[x].val);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "RENDER")) {
            fgets(txt, 1000, stdin);
            while(txt[0]==' ') memmove(txt, txt+1, strlen(txt));
            txt[strcspn(txt, "\n")] = 0;
            char out[2000] = "";
            for(char *p=txt; *p; ) {
                if(*p=='{' && strchr(p, '}')) {
                    char *end = strchr(p, '}');
                    char kbuf[100];
                    strncpy(kbuf, p+1, end-p-1);
                    kbuf[end-p-1] = 0;
                    int x = find(kbuf);
                    if(x!=-1) strcat(out, a[x].val);
                    else { strncat(out, p, end-p+1); }
                    p = end + 1;
                } else {
                    char tmp[2] = {*p, 0};
                    strcat(out, tmp);
                    p++;
                }
            }
            printf("%s\n", out);
        } else if(!strcmp(c, "VARS")) {
            for(int i=0; i<n; i++) printf("%s %s\n", a[i].key, a[i].val);
        }
    }
    return 0;
}