// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    int header_len;
    int payload_len;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *packets = NULL;

Packet *find_packet(const char *id) {
    Packet *curr = packets;
    while (curr) {
        if (strcmp(curr->id, id) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    
    Packet *p = malloc(sizeof(Packet));
    if (!p) exit(EXIT_FAILURE);
    p->id = strdup(id);
    p->header_len = hlen;
    p->payload_len = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
}

void drop_packet(const char *id) {
    Packet *curr = packets;
    Packet *prev = NULL;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else packets = curr->next;
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
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->header);
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->payload);
}

void print_stats() {
    int total = 0;
    Packet *curr = packets;
    while (curr) {
        total += curr->payload_len;
        curr = curr->next;
    }
    printf("%d\n", total);
}

void free_all() {
    Packet *curr = packets;
    while (curr) {
        Packet *next = curr->next;
        free(curr->id);
        free(curr->header);
        free(curr->payload);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[256] = {0};
        char arg4[3800] = {0};
        char arg5[3800] = {0};
        int hlen = 0, plen = 0;
        
        int matched = sscanf(line, "%31s %255s %255s %255s %3799s %3799s", cmd, arg1, arg2, arg3, arg4, arg5);
        if (matched >= 3) sscanf(arg2, "%d", &hlen);
        if (matched >= 4) sscanf(arg3, "%d", &plen);
        
        if (strcmp(cmd, "PACKET") == 0 && matched >= 5) {
            add_packet(arg1, hlen, plen, arg4, arg5);
        } else if (strcmp(cmd, "DROP") == 0) {
            drop_packet(arg1);
        } else if (strcmp(cmd, "HEADER") == 0) {
            print_header(arg1);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            print_payload(arg1);
        } else if (strcmp(cmd, "STATS") == 0) {
            print_stats();
        }
    }
    free_all();
    return 0;
}