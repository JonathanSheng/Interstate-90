#include <stdint.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <inttypes.h>
#include <getopt.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
 
#include <rte_common.h>
#include <rte_log.h>
#include <rte_memory.h>
#include <rte_eal.h>
#include <rte_launch.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_arp.h>
#include <rte_spinlock.h>
#include <rte_devargs.h>
#include <rte_byteorder.h>
#include <rte_cpuflags.h>
#include <rte_eth_bond.h>
 
#include <cstdio>
#define NB_BUF 8191 //Number of buffers held in the shared memory pool
#define MEMPOOL_CACHE_SIZE 256

static struct rte_mempool *mbuf_pool;

int main(int argc, char **argv) {

  // Environment Abstraction Layer: grants access to low-level resources through an interface
  // Creates a global struct that future rte functions rely on
  const int ret = rte_eal_init(argc, argv);
  if (ret < 0) {
    std::fprintf(stderr, "rte_eal_init failed\n");
    return 1;
  }
  
  // Create the shared memory pool = set of buffers
  // one slot in a TX/RX ring points to one of the buffers in our memory pool
  // name of mbuf pool
  // number of buffers held in shm
  // cache: own core's stack of empty buffers, helps it take or return packets to the memory pool by copying to its own stack
  //Each element in the pool is one buffer that looks like 
  // [ rte_mbuf header | private bytes | dataroom]
  // The header is DPDK default header attached to the frame
  // private bytes is the size we allocate if we want to add any data on our own that rides with the packet
  // dataroom is how much space we have for the frame in the memory pool, it is the memory needed to actually hold the frame data
  // socket id: allocate the memory pool to NUMA node on the specific lcore
  mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NB_BUF, MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
  if (mbuf_pool == NULL){
	rte_exit(EXIT_FAILURE, "Failed to create memory buffer pool \n");
  }


  //Now we create the RX/TX rings
  // net_null0 is our replacement software NIC over our hardware NIC
  uint16_t port_id {0};
  if (rte_eth_dev_get_port_by_name("net_null0", &port_id)){
	rte_exit(EXIT_FAILURE, "Error: Invalid NIC name");
  }

  struct rte_eth_conf eth_conf {};
  if (rte_eth_dev_configure(port_id, 1, 1, &eth_conf) < 0){
	rte_exit(EXIT_FAILURE, "Failed to configure Ethernet device");
  }

  //Set up and allocate the RX ring to the shared memory pool
  // We have now filled up every slot with a pointer to one buffer on the shared memory queue
  struct rte_eth_rxconf rx_conf{};
  if (rte_eth_rx_queue_setup(port_id, 0, 1024, rte_socket_id(), &rx_conf, mbuf_pool) < 0){
	rte_exit(EXIT_FAILURE, "Failed to set up RX ring");
  }

  //Set up and allocate TX ring, we don't allocate any memory from the shared buffer
  // as when we actually send a packet, we'll allocate memory
  struct rte_eth_txconf tx_conf{};
  if (rte_eth_tx_queue_setup(port_id, 0, 1024, rte_socket_id(), &tx_conf) < 0){
	rte_exit(EXIT_FAILURE, "Failed to set up TX ring");
  }

  // Start the Ethernet port
  // We can now receive and transmit payloads through the Ethernet API
  if (rte_eth_dev_start(port_id) < 0){
	rte_exit(EXIT_FAILURE, "Failed to start the Ethernet port");
  }

  rte_eal_cleanup();
  return 0;
}