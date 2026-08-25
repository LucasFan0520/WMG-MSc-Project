// F040.c
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

Packet* find_packet(const char *id) {
    Packet *cur = head;
    while (cur) {
        if (strcmp(cur->id, id) == 0) return cur;
        cur = cur->next;
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
    p->next = NULL;
    if (!head) head = p;
    else {
        Packet *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = p;
    }
}

void drop_packet(const char *id) {
    Packet *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
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

void stats() {
    int total = 0;
    Packet *cur = head;
    while (cur) {
        total += cur->payloadLen;
        cur = cur->next;
    }
    printf("%d\n", total);
}

void free_all() {
    Packet *cur = head;
    while (cur) {
        Packet *tmp = cur;
        cur = cur->next;
        free(tmp->id);
        free(tmp->header);
        free(tmp->payload);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *hlen_str = strtok(NULL, " ");
            if (!hlen_str) continue;
            char *plen_str = strtok(NULL, " ");
            if (!plen_str) continue;
            int hlen = atoi(hlen_str);
            int plen = atoi(plen_str);
            char *header = strtok(NULL, " ");
            if (!header) continue;
            char *payload = strtok(NULL, "");
            if (!payload) payload = "";
            add_packet(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = strtok(NULL, " ");
            if (id) drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = strtok(NULL, " ");
            if (id) print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = strtok(NULL, " ");
            if (id) print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats();
        }
    }
    free_all();
    return 0;
}