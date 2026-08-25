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
} Packet;

static Packet *packets = NULL;
static size_t pcount = 0, pcap = 0;
static long totalPayload = 0;

static Packet *find_packet(const char *id) {
    for (size_t i = 0; i < pcount; i++) {
        if (strcmp(packets[i].id, id) == 0)
            return &packets[i];
    }
    return NULL;
}

static void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    if (pcount == pcap) {
        pcap = pcap ? pcap * 2 : 4;
        packets = realloc(packets, pcap * sizeof(Packet));
        if (!packets) { perror("realloc"); exit(1); }
    }
    packets[pcount].id = strdup(id);
    packets[pcount].headerLen = hlen;
    packets[pcount].payloadLen = plen;
    packets[pcount].header = strdup(header);
    packets[pcount].payload = strdup(payload);
    totalPayload += plen;
    pcount++;
}

static void drop_packet(const char *id) {
    for (size_t i = 0; i < pcount; i++) {
        if (strcmp(packets[i].id, id) == 0) {
            totalPayload -= packets[i].payloadLen;
            free(packets[i].id);
            free(packets[i].header);
            free(packets[i].payload);
            for (size_t j = i; j < pcount - 1; j++)
                packets[j] = packets[j + 1];
            pcount--;
            return;
        }
    }
}

static void header_packet(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

static void payload_packet(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

static void stats_packets(void) {
    printf("%ld\n", totalPayload);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *hlen = strtok(NULL, " ");
            char *plen = strtok(NULL, " ");
            if (!hlen || !plen) continue;
            int h = atoi(hlen), p = atoi(plen);
            char *header = strtok(NULL, " ");
            if (!header) continue;
            char *payload = strtok(NULL, "");
            if (!payload) payload = "";
            add_packet(id, h, p, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = strtok(NULL, " ");
            if (id) drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = strtok(NULL, " ");
            if (id) header_packet(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = strtok(NULL, " ");
            if (id) payload_packet(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats_packets();
        }
    }
    for (size_t i = 0; i < pcount; i++) {
        free(packets[i].id);
        free(packets[i].header);
        free(packets[i].payload);
    }
    free(packets);
    return 0;
}