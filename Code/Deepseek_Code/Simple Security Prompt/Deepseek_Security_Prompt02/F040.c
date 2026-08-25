/* F040.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_packet(Packet *p) {
    free(p->id);
    free(p->header);
    free(p->payload);
    free(p);
}

void packet_add(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
}

void packet_drop(const char *id) {
    Packet *prev = NULL, *curr = packets;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else packets = curr->next;
            free_packet(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
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

void packet_stats(void) {
    int total = 0;
    Packet *p = packets;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char id[256];
    int hlen, plen;

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "PACKET") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d %d", id, &hlen, &plen) != 3) continue;
            p += strlen(id);
            while (*p == ' ') p++;
            p += (hlen >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            p += (plen >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            char *header = malloc(hlen + 1);
            strncpy(header, p, hlen);
            header[hlen] = '\0';
            p += hlen;
            char *payload = malloc(plen + 1);
            strncpy(payload, p, plen);
            payload[plen] = '\0';
            packet_add(id, hlen, plen, header, payload);
            free(header);
            free(payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) == 1) packet_drop(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) == 1) packet_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) == 1) packet_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            packet_stats();
        }
    }

    while (packets) {
        Packet *tmp = packets;
        packets = packets->next;
        free_packet(tmp);
    }
    return 0;
}