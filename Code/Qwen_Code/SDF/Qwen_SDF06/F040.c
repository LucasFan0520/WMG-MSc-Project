// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *head = NULL;

void add_packet(const char *id, int hlen, int plen, const char *hdata, const char *pdata) {
    if (strlen(hdata) < (size_t)hlen || strlen(pdata) < (size_t)plen) return;
    
    Packet *p = malloc(sizeof(Packet));
    if (!p) return;
    p->id = strdup(id);
    if (!p->id) { free(p); return; }
    
    p->header = malloc(hlen + 1);
    p->payload = malloc(plen + 1);
    if (!p->header || !p->payload) {
        free(p->id); free(p->header); free(p->payload); free(p); return;
    }
    
    strncpy(p->header, hdata, hlen);
    p->header[hlen] = '\0';
    strncpy(p->payload, pdata, plen);
    p->payload[plen] = '\0';
    
    p->next = head;
    head = p;
}

void drop_packet(const char *id) {
    Packet *curr = head;
    Packet *prev = NULL;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->id);
            free(curr->header);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_header(const char *id) {
    for (Packet *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            printf("%s\n", p->header);
            return;
        }
    }
}

void print_payload(const char *id) {
    for (Packet *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            printf("%s\n", p->payload);
            return;
        }
    }
}

void stats() {
    size_t total = 0;
    for (Packet *p = head; p; p = p->next) {
        total += strlen(p->payload);
    }
    printf("%zu\n", total);
}

void free_all() {
    Packet *p = head;
    while (p) {
        Packet *next = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[1024] = {0};
        char arg3[1024] = {0};
        char arg4[4096] = {0};
        int hlen = 0, plen = 0;
        
        int n = sscanf(line, "%63s %1023s %1023s %1023s %4095[^\n]", cmd, arg1, arg2, arg3, arg4);
        if (n < 1) continue;
        
        if (strcmp(cmd, "PACKET") == 0) {
            if (sscanf(line, "%*s %1023s %d %d %1023s %4095[^\n]", arg1, &hlen, &plen, arg3, arg4) == 5) {
                add_packet(arg1, hlen, plen, arg3, arg4);
            }
        } else if (strcmp(cmd, "DROP") == 0 && n >= 2) {
            drop_packet(arg1);
        } else if (strcmp(cmd, "HEADER") == 0 && n >= 2) {
            print_header(arg1);
        } else if (strcmp(cmd, "PAYLOAD") == 0 && n >= 2) {
            print_payload(arg1);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats();
        }
    }
    free_all();
    return 0;
}