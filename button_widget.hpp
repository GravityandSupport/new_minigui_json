#pragma once

#include "base_attr.hpp"
#include "widget.hpp"

class ButtonWidget : public Widget {
protected:
	bool m_is_move = false; // 按键在移动中
	bool m_isButtonPress = false; // 按键被按下
public:
	int button_press_bitmap=-1;
	int button_lift_bitmap =-1;

	void setButtonBitmap(int press_bitmap=-1, int lift_bitmap=-1);
	
    virtual void msg_lbutton_down(int x, int y) override;
    virtual void msg_lbutton_up(int x, int y) override;
    virtual void msg_mousemove(int x, int y) override;
	virtual void msg_paint(HDC hdc) override;

	virtual void onEvent_down(){}
	virtual void onEvent_up(){}
//	virtual void onEvent_move(){}
//	virtual void onEvent_paint(HDC hdc){}
	
    ButtonWidget(const std::string& str);
    virtual ~ButtonWidget() = default;
};
