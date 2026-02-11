#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <fcntl.h>
#include <vector>
#include <functional>
#include <tuple>
#include <array>
#include <algorithm>
#include <termios.h> //串口配置需要的头文件
#include <stdexcept> //错误头文文件
#include <thread>
#include <array>
#include <atomic>
#include <utility> // std::pair
#include <numeric>
#include <cmath>
#include <iomanip>
#include <queue>
#include <map>
#include <semaphore.h>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <pthread.h>
#include <sys/prctl.h>

// 生成 [0, max) 范围的随机整数
uint32_t get_random_urandom(uint32_t max);