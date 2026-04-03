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
#include <unordered_map>
#include <queue>
#include <map>
#include <semaphore.h>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <pthread.h>
#include <sys/prctl.h>

namespace ui{

class BaseAttr;

enum class KeyMsg {
	Single=0,
	Double=1,
	Long=2,
};

struct LongPressConfig {
	int triggerThreshold;       // 触发长按需要的持续按下消息数量（必须 > 0）
    int continueHoldCount;      // 长按后每多少个 HOLD 消息调用一次持续回调，0 = 不启用
};
struct DoubleClickConfig {
    int doubleClickThresholdMs;   // 两次抬起的时间差阈值（毫秒）
};

class LongPressDetector{
private:

public:
	using Key = uint32_t;
	using TimePoint = std::chrono::steady_clock::time_point;

	// 注册按键
    bool registerKey(Key key,
                     const LongPressConfig& config);
	bool registerKey(Key key,
                     const DoubleClickConfig& config);
	bool registerKey(Key key,
					const LongPressConfig& lconfig,
                     const DoubleClickConfig& dconfig);
	void unregisterKey(Key key);

	// 消息处理接口
    void onKeyHold(BaseAttr* win, Key key);
	void onKeyRelease(BaseAttr* win, Key key);
private:
    struct KeyLongPressState {
        int holdCount = 0;
        int continueCounter = 0;
        bool isLongPressActive = false;
		std::chrono::steady_clock::time_point lastHoldTime{};
        LongPressConfig longPressConfig;

		TimePoint lastReleaseTime{};
		bool waitingForSecondClick = false;
		DoubleClickConfig doubleClickConfig;

		bool isEnableLong = false;
		bool isEnableDouble = false;
    };

    std::unordered_map<Key, KeyLongPressState> m_registeredKeys;

    void resetState(KeyLongPressState& s);
};

}