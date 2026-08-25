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

void store_rle(const char *name, const char *encoded) {
    RLE *r = malloc(sizeof(RLE));
    r->name = strdup(name);
    r->encoded = strdup(encoded);
    r->next = head;
    head = r;
}

void decode_rle(const char *name) {
    for (RLE *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            char *p = r->encoded;
            while (*p) {
                int count = 0;
                while (*p >= '0' && *p <= '9') {
                    count = count * 10 + (*p - '0');
                    if (count > 1000000) { printf("INVALID\n"); return; }
                    p++;
                }
                if (!*p) break;
                char c = *p++;
                for (int i = 0; i < count; i++) {
                    putchar(c == ' ' ? '_' : c);
                }
            }
            putchar('\n');
            return;
        }
    }
}

void delete_rle(const char *name) {
    RLE *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name); free(p->encoded); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void list_rle() {
    for (RLE *r = head; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[8000];
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "STORE") == 0) {
            sscanf(line, "%*s %s %[^\n]", a, b);
            store_rle(a, b);
        } else if (strcmp(cmd, "DECODE") == 0) {
            sscanf(line, "%*s %s", a);
            decode_rle(a);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_rle(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_rle();
        }
    }
    while (head) {
        RLE *n = head->next;
        free(head->name); free(head->encoded); free(head);
        head = n;
    }
    return 0;
}