// SPDX-License-Identifier: GPL-2.0
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>

#define TARGET_PORT 8001
#define MAGIC_MAGIC_HEADER 0x534B5957 // "SKYW" in ASCII

SEC("xdp")
int xdp_telemetry_filter(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end) return XDP_PASS;
    if (eth->h_proto != __constant_htons(ETH_P_IP)) return XDP_PASS;

    struct iphdr *iph = (void *)(eth + 1);
    if ((void *)(iph + 1) > data_end) return XDP_PASS;
    if (iph->protocol != IPPROTO_UDP) return XDP_PASS;

    struct udphdr *udph = (void *)iph + (iph->ihl * 4);
    if ((void *)(udph + 1) > data_end) return XDP_PASS;

    // Filter incoming telemetry traffic targeting WS/UDP port 8001
    if (udph->dest == __constant_htons(TARGET_PORT)) {
        uint32_t *payload = (void *)(udph + 1);
        if ((void *)(payload + 1) > data_end) return XDP_DROP;

        // Verify magic byte signature; drop bad frames in NIC driver
        if (*payload != __constant_htonl(MAGIC_MAGIC_HEADER)) {
            return XDP_DROP;
        }
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
