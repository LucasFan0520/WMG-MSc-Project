// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Buf {
    char *name;
    unsigned char *data;
    int size;
    struct Buf *next;
} Buf;

Buf *head = NULL;

int hex_to_byte(char *s) {
    int val = 0;
    sscanf(s, "%2x", &val);
    return val;
}

int main() {
    char cmd[20];
    char name[100];
    char hex[2000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "NEW") || !strcmp(cmd, "APPEND")) {
            scanf("%s %s", name, hex);
            int len = strlen(hex);
            if (len % 2 != 0) continue;
            int bytes = len / 2;
            unsigned char *new_data = malloc(bytes);
            for (int i = 0; i < bytes; i++) {
                char tmp[3] = {hex[2*i], hex[2*i+1], 0};
                new_data[i] = hex_to_byte(tmp);
            }
            Buf *b = head;
            while (b && strcmp(b->name, name)) b = b->next;
            if (!strcmp(cmd, "NEW")) {
                if (b) {
                    free(b->data);
                    b->data = new_data;
                    b->size = bytes;
                } else {
                    Buf *nb = malloc(sizeof(Buf));
                    nb->name = strdup(name);
                    nb->data = new_data;
                    nb->size = bytes;
                    nb->next = head;
                    head = nb;
                }
            } else {
                if (b) {
                    unsigned char *combined = malloc(b->size + bytes);
                    memcpy(combined, b->data, b->size);
                    memcpy(combined + b->size, new_data, bytes);
                    free(b->data);
                    free(new_data);
                    b->data = combined;
                    b->size += bytes;
                } else {
                    Buf *nb = malloc(sizeof(Buf));
                    nb->name = strdup(name);
                    nb->data = new_data;
                    nb->size = bytes;
                    nb->next = head;
                    head = nb;
                }
            }
        } else if (!strcmp(cmd, "SLICE")) {
            int offset, length;
            scanf("%s %d %d", name, &offset, &length);
            Buf *b = head;
            while (b && strcmp(b->name, name)) b = b->next;
            if (b && offset >= 0 && length >= 0 && offset + length <= b->size) {
                for (int i = 0; i < length; i++) {
                    printf("%02x", b->data[offset + i]);
                }
                printf("\n");
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            Buf *p = head;
            Buf *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    free(p->name);
                    free(p->data);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (Buf *b = head; b; b = b->next) {
                printf("%s %d\n", b->name, b->size);
            }
        }
    }
    return 0;
}