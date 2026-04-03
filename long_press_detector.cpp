#include "long_press_detector.hpp"
#include "base_attr.hpp"

namespace ui{

void LongPressDetector::resetState(KeyLongPressState& s)
{
    s.holdCount = 0;
    s.continueCounter = 0;
    s.isLongPressActive = false;
	s.lastHoldTime = std::chrono::steady_clock::time_point{};  // 重置时间

	s.lastReleaseTime = TimePoint{};
    s.waitingForSecondClick = false;
}

bool LongPressDetector::registerKey(Key key,
                                    const LongPressConfig& config)
{
    if (config.triggerThreshold <= 0) {
        return false;
    }

    auto& state = m_registeredKeys[key];
    state.longPressConfig      = config;

    resetState(state);
	state.isEnableLong = true;
    return true;
}
bool LongPressDetector::registerKey(Key key,
                                   const DoubleClickConfig& config)
{
    auto& state = m_registeredKeys[key];
    state.doubleClickConfig = config;
    resetState(state);
	state.isEnableDouble = true;
    return true;
}
bool LongPressDetector::registerKey(Key key,
					const LongPressConfig& lconfig,
                     const DoubleClickConfig& dconfig){
	auto& state = m_registeredKeys[key];

    // 长按配置
    if (lconfig.triggerThreshold > 0) {
        state.longPressConfig = lconfig;
    }

    // 双击配置
    state.doubleClickConfig = dconfig;

    resetState(state);
	state.isEnableLong = true;
	state.isEnableDouble = true;
    return true;
}
void LongPressDetector::unregisterKey(Key key)
{
    m_registeredKeys.erase(key);
}

void LongPressDetector::onKeyHold(BaseAttr* win, Key key)
{
	LOG_DEBUG("long", key);
    auto it = m_registeredKeys.find(key);
    if (it == m_registeredKeys.end()) {
        return;
    }

    auto& s = it->second;
	if(s.isEnableLong==false) {return;}

	auto now = std::chrono::steady_clock::now();
	auto gap_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - s.lastHoldTime).count(); // 计算这次 HOLD 与上次 HOLD 的时间间隔（毫秒）
	LOG_DEBUG("long", key, s.holdCount, gap_ms, s.isLongPressActive);

	if (s.isLongPressActive && gap_ms > 500) {        // 200ms 可根据你的框架实际 HOLD 频率调整
        LOG_DEBUG("long", key, "检测到长按已抬起（时间间隔过大）");
        resetState(s);
        // 注意：这里不 return，继续处理本次 HOLD（开始新的统计）
    }
	s.lastHoldTime = now; // 更新最后一次 HOLD 时间
	
    s.holdCount++;
	LOG_DEBUG("long", key, s.holdCount, s.isLongPressActive);

    // 触发长按开始
    if (s.holdCount >= s.longPressConfig.triggerThreshold && !s.isLongPressActive) {
        s.isLongPressActive = true;

		win->key_long_press(key, static_cast<uint32_t>(KeyMsg::Long));
	
        s.continueCounter = 0;
    }

    // 长按周期性持续回调（可选）
    if (s.isLongPressActive &&
        s.longPressConfig.continueHoldCount > 0){
        s.continueCounter++;

        if (s.continueCounter >= s.longPressConfig.continueHoldCount) {
			win->key_long_press(key, static_cast<uint32_t>(KeyMsg::Long));
			
            s.continueCounter = 0;
        }
    }
}

void LongPressDetector::onKeyRelease(BaseAttr* win, Key key)
{
    auto it = m_registeredKeys.find(key);
    if (it == m_registeredKeys.end()) return;

    auto& s = it->second;
    auto now = std::chrono::steady_clock::now();

    // ==================== 双击判断（新增核心逻辑） ====================
    if (s.isEnableDouble && s.waitingForSecondClick) {
        auto gap = std::chrono::duration_cast<std::chrono::milliseconds>(now - s.lastReleaseTime).count();

        if (gap <= s.doubleClickConfig.doubleClickThresholdMs) {
			win->key_long_press(key, static_cast<uint32_t>(KeyMsg::Double));
            resetState(s);
            return;
        }
    }

    win->key_long_press(key, static_cast<uint32_t>(KeyMsg::Single));

    // 更新状态，进入等待第二次点击窗口
    s.lastReleaseTime = now;
    s.waitingForSecondClick = true;

    resetState(s);   // 注意：双击等待状态已在上面处理，这里重置长按部分
}

}