// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct RLE {
    char *name;
    char *encoded;
    struct RLE *next;
} RLE;

RLE *head = NULL;

int main() {
    char cmd[20];
    char name[100];
    char encoded[2000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "STORE")) {
            scanf("%s %s", name, encoded);
            RLE *r = malloc(sizeof(RLE));
            r->name = strdup(name);
            r->encoded = strdup(encoded);
            r->next = head;
            head = r;
        } else if (!strcmp(cmd, "DECODE")) {
            scanf("%s", name);
            for (RLE *r = head; r; r = r->next) {
                if (!strcmp(r->name, name)) {
                    char *p = r->encoded;
                    int valid = 1;
                    while (*p) {
                        int count = 0;
                        while (*p >= '0' && *p <= '9') {
                            count = count * 10 + (*p - '0');
                            p++;
                            if (count > 1000000) {
                                valid = 0;
                                break;
                            }
                        }
                        if (!valid || !*p) break;
                        char c = *p++;
                        for (int i = 0; i < count; i++) {
                            if (c == ' ') printf("_");
                            else printf("%c", c);
                        }
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            RLE *p = head;
            RLE *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    free(p->name);
                    free(p->encoded);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (RLE *r = head; r; r = r->next) {
                printf("%s\n", r->name);
            }
        }
    }
    return 0;
}