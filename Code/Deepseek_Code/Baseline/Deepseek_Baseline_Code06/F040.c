/* F040.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    int headerLen;
    int payloadLen;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *head = NULL;
long long total_payload = 0;

Packet* find_packet(const char *id) {
    Packet *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0) return;
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = head;
    head = p;
    total_payload += plen;
}

void drop_packet(const char *id) {
    Packet *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            total_payload -= p->payloadLen;
            free(p->id);
            free(p->header);
            free(p->payload);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void print_header(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

void print_stats() {
    printf("%lld\n", total_payload);
}

void free_all() {
    while (head) {
        Packet *tmp = head;
        head = head->next;
        free(tmp->id);
        free(tmp->header);
        free(tmp->payload);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256];
        int hlen, plen;
        char rest[4096];
        int n = sscanf(line, "%31s %255s %d %d %[^\n]", cmd, id, &hlen, &plen, rest);
        if (strcmp(cmd, "PACKET") == 0 && n == 5) {
            if (hlen < 0 || plen < 0) continue;
            if (hlen + plen > (int)strlen(rest)) continue;
            char header[4096], payload[4096];
            strncpy(header, rest, hlen);
            header[hlen] = '\0';
            strcpy(payload, rest + hlen);
            if ((int)strlen(payload) != plen) continue;
            add_packet(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0 && n == 2) {
            drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0 && n == 2) {
            print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0 && n == 2) {
            print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0 && n == 1) {
            print_stats();
        }
    }
    free_all();
    return 0;
}