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

Packet *packets = NULL;

Packet *find_packet(const char *id) {
    Packet *p = packets;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0) return;
    if (strlen(header) != hlen || strlen(payload) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
}

void drop_packet(const char *id) {
    Packet *prev = NULL, *cur = packets;
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            if (prev) prev->next = cur->next;
            else packets = cur->next;
            free(cur->id);
            free(cur->header);
            free(cur->payload);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
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

void stats(void) {
    int total = 0;
    Packet *p = packets;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_packets(void) {
    while (packets) {
        Packet *tmp = packets;
        packets = packets->next;
        free(tmp->id);
        free(tmp->header);
        free(tmp->payload);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char id[256];
        int hlen, plen;
        int n = sscanf(line, "%s %s %d %d", cmd, id, &hlen, &plen);
        if (strcmp(cmd, "PACKET") == 0) {
            if (n < 4) continue;
            char *p = line + strlen(cmd) + 1 + strlen(id) + 1;
            while (*p == ' ') p++;
            char *hend = p;
            while (*hend && *hend != ' ') hend++;
            if (!*hend) continue;
            *hend = '\0';
            char *header = p;
            p = hend + 1;
            while (*p == ' ') p++;
            char *pay = p;
            while (*pay && *pay != ' ') pay++;
            if (*pay) *pay = '\0';
            char *payload = p;
            add_packet(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (n < 2) continue;
            drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (n < 2) continue;
            print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (n < 2) continue;
            print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats();
        }
    }
    free_packets();
    return 0;
}