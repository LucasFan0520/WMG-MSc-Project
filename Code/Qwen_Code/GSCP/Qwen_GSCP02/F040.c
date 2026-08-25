/* F040.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    int headerLength;
    int payloadLength;
    char *header;
    char *payload;
} Packet;

static Packet *packets = NULL;
static int pcount = 0;
static int pcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_packet(const char *id) {
    for (int i = 0; i < pcount; i++)
        if (strcmp(packets[i].id, id) == 0) return i;
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "PACKET") == 0) {
            char id[256] = {0};
            int hlen = 0, plen = 0;
            int off2 = 0, off3 = 0, off4 = 0;
            if (sscanf(line + offset, "%255s%n %d%n %d%n", id, &off2, &hlen, &off3, &plen, &off4) != 3) continue;
            if (hlen < 0 || plen < 0) continue;
            const char *rest = line + offset + off2 + off3 + off4;
            while (*rest == ' ') rest++;
            if ((int)strlen(rest) < hlen + plen) continue;
            char *header = malloc(hlen + 1);
            char *payload = malloc(plen + 1);
            if (!header || !payload) { free(header); free(payload); continue; }
            memcpy(header, rest, hlen);
            header[hlen] = '\0';
            memcpy(payload, rest + hlen, plen);
            payload[plen] = '\0';
            
            int idx = find_packet(id);
            if (idx >= 0) {
                free(packets[idx].header);
                free(packets[idx].payload);
                packets[idx].headerLength = hlen;
                packets[idx].payloadLength = plen;
                packets[idx].header = header;
                packets[idx].payload = payload;
            } else {
                if (pcount >= pcap) {
                    pcap = pcap == 0 ? 8 : pcap * 2;
                    packets = realloc(packets, sizeof(Packet) * pcap);
                }
                packets[pcount].id = dup_str(id);
                packets[pcount].headerLength = hlen;
                packets[pcount].payloadLength = plen;
                packets[pcount].header = header;
                packets[pcount].payload = payload;
                pcount++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[256] = {0};
            if (sscanf(line + offset, "%255s", id) != 1) continue;
            int idx = find_packet(id);
            if (idx < 0) continue;
            free(packets[idx].id);
            free(packets[idx].header);
            free(packets[idx].payload);
            for (int i = idx; i < pcount - 1; i++) packets[i] = packets[i + 1];
            pcount--;
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[256] = {0};
            if (sscanf(line + offset, "%255s", id) != 1) continue;
            int idx = find_packet(id);
            if (idx >= 0) printf("%s\n", packets[idx].header);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[256] = {0};
            if (sscanf(line + offset, "%255s", id) != 1) continue;
            int idx = find_packet(id);
            if (idx >= 0) printf("%s\n", packets[idx].payload);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "STATS") == 0) {
            long total = 0;
            for (int i = 0; i < pcount; i++) {
                total += packets[i].payloadLength;
            }
            printf("%ld\n", total);
        }
    }
    for (int i = 0; i < pcount; i++) {
        free(packets[i].id);
        free(packets[i].header);
        free(packets[i].payload);
    }
    free(packets);
    return 0;
}