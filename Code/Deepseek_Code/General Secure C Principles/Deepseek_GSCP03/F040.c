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

Packet *find_packet(const char *id) {
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
    if (!p) return;
    p->id = strdup(id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = head;
    head = p;
}

void drop_packet(const char *id) {
    Packet *prev = NULL, *curr = head;
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

void header_packet(const char *id) {
    Packet *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->header);
}

void payload_packet(const char *id) {
    Packet *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->payload);
}

void stats_packets(void) {
    int total = 0;
    Packet *p = head;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
    Packet *p = head;
    while (p) {
        Packet *tmp = p;
        p = p->next;
        free(tmp->id);
        free(tmp->header);
        free(tmp->payload);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256];
            int hlen, plen;
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d %d", id, &hlen, &plen) == 3) {
                char *header = p;
                while (*header && *header != ' ') header++;
                while (*header && *header == ' ') header++;
                while (*header && *header != ' ') header++;
                while (*header && *header == ' ') header++;
                // header starts here, need to extract exactly hlen chars
                char header_buf[1024];
                int copied = 0;
                while (*header && *header != ' ' && copied < hlen) {
                    header_buf[copied++] = *header++;
                }
                header_buf[copied] = '\0';
                // now payload
                while (*header == ' ') header++;
                char payload_buf[1024];
                copied = 0;
                while (*header && *header != ' ' && copied < plen) {
                    payload_buf[copied++] = *header++;
                }
                payload_buf[copied] = '\0';
                if (copied == hlen && (int)strlen(header_buf) == hlen && (int)strlen(payload_buf) == plen) {
                    add_packet(id, hlen, plen, header_buf, payload_buf);
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            drop_packet(p);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            header_packet(p);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            payload_packet(p);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats_packets();
        }
    }
    free_all();
    return 0;
}