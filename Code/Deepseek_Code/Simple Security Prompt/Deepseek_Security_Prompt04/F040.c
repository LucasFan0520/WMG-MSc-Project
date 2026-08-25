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

Packet *find_packet(char *id) {
    for (Packet *p = head; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

void add_packet(char *id, int hlen, int plen, char *header, char *payload) {
    Packet *p = find_packet(id);
    if (p) {
        free(p->header);
        free(p->payload);
        p->headerLen = hlen;
        p->payloadLen = plen;
        p->header = strdup(header);
        p->payload = strdup(payload);
        return;
    }
    Packet *new = malloc(sizeof(Packet));
    new->id = strdup(id);
    new->headerLen = hlen;
    new->payloadLen = plen;
    new->header = strdup(header);
    new->payload = strdup(payload);
    new->next = head;
    head = new;
}

void drop_packet(char *id) {
    Packet *prev = NULL;
    for (Packet *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->id);
            free(p->header);
            free(p->payload);
            free(p);
            return;
        }
        prev = p;
    }
}

void print_header(char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

void print_payload(char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

void stats(void) {
    int total = 0;
    for (Packet *p = head; p; p = p->next)
        total += p->payloadLen;
    printf("%d\n", total);
}

void free_all(void) {
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256], arg4[1024], arg5[1024];
        int n = sscanf(line, "%15s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, arg4);
        if (n < 1) continue;
        if (strcmp(cmd, "PACKET") == 0 && n == 5) {
            // parse headerLen and payloadLen
            int hlen = atoi(arg2);
            int plen = atoi(arg3);
            // header is first hlen chars of arg4? arg4 is the rest of line after the IDs? Actually format: PACKET id headerLength payloadLength header payload
            // We need to split header and payload from arg4.
            // arg4 contains header and payload, but we have only one string. Use the length fields.
            char *data = arg4;
            if (strlen(data) < hlen + plen) {
                // invalid
            } else {
                char header[1024], payload[1024];
                strncpy(header, data, hlen);
                header[hlen] = '\0';
                strcpy(payload, data + hlen);
                add_packet(arg1, hlen, plen, header, payload);
            }
        } else if (strcmp(cmd, "DROP") == 0 && n == 2) {
            drop_packet(arg1);
        } else if (strcmp(cmd, "HEADER") == 0 && n == 2) {
            print_header(arg1);
        } else if (strcmp(cmd, "PAYLOAD") == 0 && n == 2) {
            print_payload(arg1);
        } else if (strcmp(cmd, "STATS") == 0 && n == 1) {
            stats();
        }
    }
    free(line);
    free_all();
    return 0;
}