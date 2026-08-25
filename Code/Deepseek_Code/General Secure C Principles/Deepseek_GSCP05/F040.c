// F040.c
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

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0) return;
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    if (!p) return;
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
    if (!p) return;
    printf("%s\n", p->header);
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->payload);
}

void print_stats(void) {
    int total = 0;
    Packet *p = packets;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
    Packet *p = packets;
    while (p) {
        Packet *next = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            char *space = strchr(id, ' ');
            if (!space) continue;
            *space = '\0';
            int hlen, plen;
            char *rest = space + 1;
            while (*rest == ' ') rest++;
            if (sscanf(rest, "%d %d", &hlen, &plen) != 2) continue;
            // find header and payload
            char *hstart = strchr(rest, ' ');
            if (!hstart) continue;
            hstart = strchr(hstart + 1, ' ');
            if (!hstart) continue;
            while (*hstart == ' ') hstart++;
            char *header = hstart;
            // header length is hlen, so payload starts after that
            if ((int)strlen(header) < hlen) continue;
            char *payload_start = header + hlen;
            if (*payload_start == ' ') payload_start++;
            // payload length is plen
            if ((int)strlen(payload_start) < plen) continue;
            char *payload = payload_start;
            // we need to split; we can use temporary strings
            char *hcopy = malloc(hlen + 1);
            char *pcopy = malloc(plen + 1);
            if (!hcopy || !pcopy) { free(hcopy); free(pcopy); continue; }
            strncpy(hcopy, header, hlen);
            hcopy[hlen] = '\0';
            strncpy(pcopy, payload, plen);
            pcopy[plen] = '\0';
            add_packet(id, hlen, plen, hcopy, pcopy);
            free(hcopy);
            free(pcopy);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *id = p;
            char *space = strchr(id, ' ');
            if (space) *space = '\0';
            drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            char *space = strchr(id, ' ');
            if (space) *space = '\0';
            print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *id = p;
            char *space = strchr(id, ' ');
            if (space) *space = '\0';
            print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            print_stats();
        }
    }
    free_all();
    return 0;
}