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

void packet_add(const char *id, int hlen, int plen, const char *hdr, const char *pld) {
    if (find_packet(id)) return;
    if ((int)strlen(hdr) != hlen || (int)strlen(pld) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    if (!p) return;
    p->id = strdup(id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = strdup(hdr);
    p->payload = strdup(pld);
    p->next = packets;
    packets = p;
}

void packet_drop(const char *id) {
    Packet *p = packets, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else packets = p->next;
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

void packet_header(const char *id) {
    Packet *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->header);
}

void packet_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->payload);
}

void stats_total(void) {
    int total = 0;
    Packet *p = packets;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256], arg4[2048], arg5[2048];
        int n = sscanf(line, "%15s %255s %255s %255s %2047s %2047s", cmd, arg1, arg2, arg3, arg4, arg5);
        if (strcmp(cmd, "PACKET") == 0 && n >= 6) {
            packet_add(arg1, atoi(arg2), atoi(arg3), arg4, arg5);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 2) {
            packet_drop(arg1);
        } else if (strcmp(cmd, "HEADER") == 0 && n >= 2) {
            packet_header(arg1);
        } else if (strcmp(cmd, "PAYLOAD") == 0 && n >= 2) {
            packet_payload(arg1);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats_total();
        }
    }
    while (packets) {
        Packet *tmp = packets;
        packets = packets->next;
        free(tmp->id);
        free(tmp->header);
        free(tmp->payload);
        free(tmp);
    }
    return 0;
}