#include "key_window.hpp"

void KeyWidget::registerWidget(KeyWindowWidget &widget){
	Widget::registerWidget(widget);
	key_layout.emplace_back();          // 添加空行
    key_layout.back().push_back(&widget); // 该行只包含此控件
    widget.setCanFocus(true);
}

void KeyWidget::registerWidget(size_t row, KeyWindowWidget &widget){
	Widget::registerWidget(widget);
	// 策略：自动填充缺失的中间行（空行）
    if (row >= key_layout.size()) {
        key_layout.resize(row + 1);
    }
    key_layout[row].push_back(&widget);
	widget.setCanFocus(true);
}

bool KeyWidget::setFocus(size_t row, size_t col) {
    if (row >= key_layout.size() || col >= key_layout[row].size()) {
        return false;
    }
    
    // 失去焦点回调（如果有）
    if (hasFocus()) {
        KeyWindowWidget* old = getCurrentWidget();
        if (old) old->onFocusLost();
    }
    
    current_focus = {static_cast<int>(row), static_cast<int>(col)};
    
    // 获得焦点回调
    KeyWindowWidget* current = getCurrentWidget();
    if (current) current->onFocusGained();
    
    return true;
}
bool KeyWidget::setFocus(Widget* widget) {
    if (!widget) return false;
    auto pos = findWidgetPosition(widget);
    if (pos.first == -1) return false;
    return setFocus(pos.first, pos.second);
}
bool KeyWidget::trySetFocus(int row, int col) {
    if (row < 0 || row >= static_cast<int>(key_layout.size())) {
        return false;
    }
    if (col < 0 || col >= static_cast<int>(key_layout[row].size())) {
        return false;
    }
    
    KeyWindowWidget* widget = key_layout[row][col];
    if (widget && widget->canFocus()) {
        return setFocus(row, col);
    }
    return false;
}
KeyWindowWidget* KeyWidget::getCurrentWidget() const {
    if (!hasFocus()) return nullptr;
    return key_layout[current_focus.first][current_focus.second];
}
void KeyWidget::clearFocus() {
   if (hasFocus()) {
       KeyWindowWidget* current = getCurrentWidget();
       if (current) current->onFocusLost();
   }
    current_focus = {-1, -1};
}
bool KeyWidget::focusFirst() {
    if (key_layout.empty()) return false;

    for (size_t r = 0; r < key_layout.size(); ++r) {
        for (size_t c = 0; c < key_layout[r].size(); ++c) {
            if (trySetFocus(static_cast<int>(r), static_cast<int>(c))) {
                return true;
            }
        }
    }
    return false;
}
bool KeyWidget::focusLast() {
    if (key_layout.empty()) return false;

    for (int r = static_cast<int>(key_layout.size()) - 1; r >= 0; --r) {
        for (int c = static_cast<int>(key_layout[r].size()) - 1; c >= 0; --c) {
            if (trySetFocus(r, c)) {
                return true;
            }
        }
    }
    return false;
}
bool KeyWidget::focusUp() {
    if (!hasFocus()) return focusFirst();

    int row = current_focus.first;
    int col = current_focus.second;

    // 优先：向上找同一列 第一个可焦点的
    for (int r = row - 1; r >= 0; --r) {
        if (trySetFocus(r, col)) {
            return true;
        }
    }

    // 没找到同列 → 尝试上一行的最右边
    if (row > 0) {
        int prev_row = row - 1;
        int last_col = static_cast<int>(key_layout[prev_row].size()) - 1;
        if (last_col >= 0) {
            if (trySetFocus(prev_row, last_col)) {
                return true;
            }
        }
    }

    // 实在找不到 → 跳到最后一个可焦点的
    return focusLast();
}
bool KeyWidget::focusDown() {
    if (!hasFocus()) return focusFirst();

    int row = current_focus.first;
    int col = current_focus.second;

    // 优先：向下找同一列 第一个可焦点的
    for (size_t r = row + 1; r < key_layout.size(); ++r) {
        if (trySetFocus(static_cast<int>(r), col)) {
            return true;
        }
    }

    // 没找到同列 → 尝试下一行的最右边（你当前代码风格）
    if (row + 1 < static_cast<int>(key_layout.size())) {
        int next_row = row + 1;
        size_t last = key_layout[next_row].size();
        if (last > 0) {
            if (trySetFocus(next_row, static_cast<int>(last) - 1)) {
                return true;
            }
        }
    }

    return focusFirst();
}
bool KeyWidget::focusLeft() {
    if (!hasFocus()) return false;

    int row = current_focus.first;
    int col = current_focus.second;

    // 同一行向左找第一个可焦点的
    for (int c = col - 1; c >= 0; --c) {
        if (trySetFocus(row, c)) {
            return true;
        }
    }

    // 到达行首 → 上一行最右（找可焦点的）
    if (row > 0) {
        int prev_row = row - 1;
        for (int c = static_cast<int>(key_layout[prev_row].size()) - 1; c >= 0; --c) {
            if (trySetFocus(prev_row, c)) {
                return true;
            }
        }
    }

    // 找不到 → 跳全局最后一个可焦点
    return focusLast();
}

bool KeyWidget::focusRight() {
    if (!hasFocus()) return false;

    int row = current_focus.first;
    int col = current_focus.second;
    size_t cols = key_layout[row].size();

    // 同一行向右找第一个可焦点的
    for (size_t c = col + 1; c < cols; ++c) {
        if (trySetFocus(row, static_cast<int>(c))) {
            return true;
        }
    }

    // 到达行尾 → 下一行最左（找可焦点的）
    if (static_cast<size_t>(row) + 1 < key_layout.size()) {
        int next_row = row + 1;
        for (size_t c = 0; c < key_layout[next_row].size(); ++c) {
            if (trySetFocus(next_row, static_cast<int>(c))) {
                return true;
            }
        }
    }

    // 找不到 → 跳全局第一个可焦点
    return focusFirst();
}
bool KeyWidget::focusMoveControl(WPARAM wParam, LPARAM lParam)  {
    switch(wParam){
		case SCANCODE_CURSORBLOCKUP:
			focusUp();
			return true;
		case SCANCODE_CURSORBLOCKDOWN:
			return focusDown();
			return true;
		case SCANCODE_CURSORBLOCKLEFT:
			return focusLeft();
			return true;
		case SCANCODE_CURSORBLOCKRIGHT:
			return focusRight();
			return true;
		case SCANCODE_ESCAPE: // 默认关闭窗口消息
			if(this->parentWindow){this->parentWindow->Close();}
			return true;
		default:break;
    }
	
	return false;
}
size_t KeyWidget::getColumnCount(size_t row) const {
    if (row >= key_layout.size()) return 0;
    return key_layout[row].size();
}
bool KeyWidget::sendMessageToCurrent(void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam){
	Widget* current = getCurrentWidget();
    if (!current) {
		LOG_INFO("无效", "当前无有效焦点");
        return false;  // 无焦点控件
    }

	(current->*func)(wParam, lParam);
	return true;
}
std::pair<int, int> KeyWidget::findWidgetPosition(Widget* widget) const {
    if (!widget) return {-1, -1};
    
    for (size_t r = 0; r < key_layout.size(); ++r) {
        const auto& row = key_layout[r];
        for (size_t c = 0; c < row.size(); ++c) {
            if (row[c] == widget) {
                return {static_cast<int>(r), static_cast<int>(c)};
            }
        }
    }
    return {-1, -1};
}

