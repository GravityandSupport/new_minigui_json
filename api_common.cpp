#include "api_common.hpp"


// 生成 [0, max) 范围的随机整数
uint32_t get_random_urandom(uint32_t max) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        perror("open /dev/urandom failed");
        return 0;
    }

    uint32_t rand_val;
    if (read(fd, &rand_val, sizeof(rand_val)) != sizeof(rand_val)) {
        perror("read /dev/urandom failed");
        close(fd);
        return 0;
    }
    close(fd);

    return max ? (rand_val % max) : rand_val;
}