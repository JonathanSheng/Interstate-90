#include <rte_eal.h>
#include <rte_lcore.h>

#include <cstdio>

int main(int argc, char **argv) {
  const int ret = rte_eal_init(argc, argv);
  if (ret < 0) {
    std::fprintf(stderr, "rte_eal_init failed\n");
    return 1;
  }

  std::printf("EAL ok, lcores=%u\n", rte_lcore_count());
  rte_eal_cleanup();
  return 0;
}
