#include "long_press_detector.hpp"
#include "base_attr.hpp"

namespace ui{

void LongPressDetector::resetState(KeyLongPressState& s)
{
    s.holdCount = 0;
    s.continueCounter = 0;
    s.isLongPressActive = false;
	s.lastHoldTime = std::chrono::steady_clock::time_point{};  // 重置时间

//	s.lastReleaseTime = TimePoint{};
//    s.waitingForSecondClick = false;
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

	// 如果没有开启双击功能，直接走单击逻辑
    if (!s.isEnableDouble) {
        win->key_long_press(key, static_cast<uint32_t>(KeyMsg::Single));
        resetState(s);
        return;
    }
	LOG_WARN("LONG");
    // ==================== 开启双击后的核心逻辑 ====================
    if (s.waitingForSecondClick) {
        // 【情况 A】这是第二次抬起！
        auto gap = std::chrono::duration_cast<std::chrono::milliseconds>(now - s.lastReleaseTime).count();

        // 再次安全检查，如果在阈值内，触发双击
        if (gap <= s.doubleClickConfig.doubleClickThresholdMs) {
            LOG_WARN("long", key, "成功触发排他性双击！间隔ms:", gap);
            win->key_long_press(key, static_cast<uint32_t>(KeyMsg::Double));
        }
        
        // 无论是双击成功还是异常超时，双击事件已消费，复位所有状态
        s.waitingForSecondClick = false;
    } 
    else {
        // 【情况 B】这是第一次抬起！
        LOG_WARN("long", key, "第一次点击抬起，拦截单击信号，进入双击等待期...");
        s.lastReleaseTime = now;
        s.waitingForSecondClick = true; // 开启标记，交给 onTime 去倒计时
    }

    // 更新状态，进入等待第二次点击窗口
//    s.lastReleaseTime = now;
//    s.waitingForSecondClick = true;

    resetState(s);   // 注意：双击等待状态已在上面处理，这里重置长按部分
}

void LongPressDetector::onTime(BaseAttr* win)
{
    auto now = std::chrono::steady_clock::now();

    // 遍历所有注册过的按键状态
    for (auto& pair : m_registeredKeys) {
        Key key = pair.first;
        auto& s = pair.second;

        // 如果该按键开启了双击，并且当前正在等待第二击
        if (s.isEnableDouble && s.waitingForSecondClick) {
            auto gap = std::chrono::duration_cast<std::chrono::milliseconds>(now - s.lastReleaseTime).count();

            // 一旦当前时间距离第一次抬起的时间超过了阈值 (如 300ms)
            if (gap > s.doubleClickConfig.doubleClickThresholdMs) {
                LOG_WARN("long", key, "双击超时，确认用户为单击。延迟触发 Single 信号");
                
                // 此时才安全地补发单击信号
                win->key_long_press(key, static_cast<uint32_t>(KeyMsg::Single));
                
                // 触发后关闭等待状态
                s.waitingForSecondClick = false;
            }
        }
    }
}


}