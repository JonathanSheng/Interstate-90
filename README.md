### Context
This project is an educational project to help understand more about Data Plane Development Kit (DPDK).
OSI model is important, as when we send data we go from top to bottom, adding more information at each layer (encapsulation).
Naturally, when we receive data, we move bottom up, where we strip away relevent information at each layer (decapsulation).

### Lifecycle of a packet in Linux (RX)

-1) Quadrature Amplitude Modulation explains the specifics on how we send the data, such as the exact pulses or flashes of light.

0) The sender either goes through wired or wireless. The wired connection is routed straight to the NIC's port and the wireless connection is picked up by the NIC's antenna.

1) Machine receives a packet through a cable, wireless, etc and is routed directly to the Network Interface Card (NIC). We can think of NIC as the interface between between the physical and data link layer. The NIC receives pulses such as voltage or pulses of light, and translates these messages into bytes. From bytes, we store in a frame. We can think of a frame as an envelope of data, that includes destination, source, type, payload, checksum. 
A frame is one complete unit of data. The NIC needs to determine in the long stream of bits where the start and end of a message is and convert into a frame.

2) Now, the NIC needs to move the frame into RAM. The RX (receive) ring is just a fixed size array that wraps around in a circle. Each RX ring slot holds an address that points to a buffer, and that buffer is a single contiguous block of free memory. So, the NIC pulls the address off the next slot in RX ring and then writes the frame to that block of memory. The NIC then sets the `done` bit and frame length in the ring descriptor and raises a hardware interrupt. 
The NIC uses DMA (Direct Memory Access) to perform the writes, which avoids CPU copying the data. Note: the RX ring and the buffers it points to live in the RAM.

3) The interrupt arrives at the CPU core and the CPU saves state, and jumps into the NIC interrupt handler. While the handler runs, the CPU core is blocked by the interrupt. Additionally, we need to disable (mask) further interrupts from the RX queue, but we still continue to receive messages from NIC. 
Then we call `napi_schedule()`, indicating the queue has work; poll it soon. This adds the queue to the core's poll list and flags a `softirq` to run. Then the handler returns and lets the CPU free.
Note: An RX ring interrupt implies that frames are waiting to be processed. We don't turn it on yet because it's possible there's more frames in the ring and we don't want to constantly interrupt. We'll turn on interrupts again when the ring is empty, indicating there's no more frames for the time being.

4) A `softirq` (software interrupt) is deferred kernel work that runs at a slightly lower priority than hardware interrupts. Usually this runs on the same CPU core right after the hardware interrupt. 
`NET_RX_SOFTIRQ` is the name of the software interrupt/softirq and its handler `net_rx_action()` runs the NAPI poll loop. 
The kernel calls `net_rx_action()` goes through the core's poll list and calls each queue's driver poll function. The driver poll function traverses the RX ring descriptors and wraps each buffer into an `sk_buff` (a data structure used to represent a network packet). We set a max of 64 frames per trip. If we have fewer than 64 frames, then we call `napi_complete_done()`, re-enabling the interrupt because this means we don't have any more buffers to process. Else, we need to continue polling the frames from RX ring. The kernel looks at the `done` bit and if we see the next slot is not set, we are done.

5) Now that we have the `sk_buff` containing the frame's data, the kernel figures out which protocol handler receives the frame: IPv4, IPv6, ARP, etc.
First we call `napi_gro_receive()`.
If this packet can be merged with another, we just merge them into one big `sk_buff`. Essentially, we just batch the frames that belong to the same flow. A frame can be considered a part of an existing flow if the frames share the same source/dest IP, same ports, same protocol, etc. This batching helps avoid the constant checks as the packet moves up the stack.
If not in the same flow, then we call `__netif_receive_skb_core()`, which hands it off to the proper handler `ip_rcv()` vs `ipv6_rcv()` vs `arp_rcv()`.

6) `ip_rcv()` validates IP headers: checksum, version, min length. 
`NF_HOOK()`,  runs check on every incoming packet before routing (`PRE_ROUTING`) and rewrites (DNAT) destination IPs or ports if necessary (port forwarding). Packets can also be dropped here if they fail the checks
`ip_route_input_noref()` checks the destination IP in its routing table and attaches its decision on local vs forward to the packet.
`dst_input()` uses the decision from `ip_route_input_noref()` and either continues up the stack and continue through netfilter OR
if we forward this frame to another machine. If this machine is a forwarder, we need to go through `FORWARD` on netfilter and place this packet onto the NIC's TX ring to send out. 

7) TODO: DPDK focuses more on the parts before this

### How does DPDK differ?

1) Unbind the NIC from the kernel and bind to VFIO, granting our program direct access. Additionally, the RX ring and the buffers are owned by the process. For each buffer, DPDK also builds the metadata required for `sk_buff`.

2) The NIC still receives the signal and DMA into the RX ring. However, we don't ever raise an interrupt.

3) We already have a dedicated core spinning in a loop calling `rte_eth_rx_burst()`. This replaces the need for the interrupt.

4) Within `rte_eth_rx_burst()`, the PMD (Poll Mode Driver) takes the mbuf and passes the packet along and refreshes the slot.

### Tradeoffs

Throughput: DPDK much better
Latency: DPDK
CPU utilization: Linux at 0% when no interrupts, DPDK at 100% for polling core

### Terms
**Network Interface Card** (NIC) is a hardware component that connects a computer or server to a network. It takes digital data from the computer and turns it into electrical, optical, or radio signals. It also works to translate those signals back into data.
See https://www.geeksforgeeks.org/computer-networks/nic-full-form/

**Ethernet Frame** is a fundamental unit of data transmission at Layer 2 of OSI. This wraps an IP packet and provides information, such as sender and recipient hardware addresses.

**Huge Pages** are larger than normal pages of memory. DPDK requires hugepages to minimize TLB misses. 
### Processing Packets in Linux
First, a packet is sent to a Network Interface Card.


### Running this project
Using Lima to run Linux VM through CLI
```
limactl shell Interstate90
limactl stop Interstate90

echo 1024 | sudo tee /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
grep Huge /proc/meminfo

sudo ./build/interstate90 -l 0 --vdev=net_null0 --no-pci
```

Installation:
```
sudo apt update

sudo apt install -y build-essential meson ninja-build pkg-config \
  g++ libdpdk-dev libpcap-dev python3 python3-pyelftools

echo 1024 | sudo tee /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
grep Huge /proc/meminfo
pkg-config --modversion libdpdk

meson build
meson compile -C build

```
