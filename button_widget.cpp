#include "button_widget.hpp"
#include "base_window.hpp"

ButtonWidget::ButtonWidget(const std::string& str)
    : Widget(str){

}


void ButtonWidget::msg_lbutton_down(int x, int y)  {
    if(PtInRect(&rc, x, y)){
		m_isButtonPress = true;
		m_is_move = false;

		onEvent_down();
		
		updateDirtyArea();
    }
	Widget::msg_lbutton_down(x, y);
}
void ButtonWidget::msg_lbutton_up(int x, int y)  {
	if(m_isButtonPress==true){
		if(m_is_move==false && PtInRect(&rc, x, y)){
			onEvent_up();
	    }
	
		m_isButtonPress = false;
		updateDirtyArea();
	}
	Widget::msg_lbutton_up(x, y);
}
void ButtonWidget::msg_mousemove(int x, int y)  {
	m_is_move = true;
    if(m_isButtonPress==false && PtInRect(&rc, x, y)){
		m_isButtonPress = true;
		updateDirtyArea();
    }else if(m_isButtonPress==true && !PtInRect(&rc, x, y)){
		m_isButtonPress = false;
		updateDirtyArea();
    }
	Widget::msg_mousemove(x, y);
}

void ButtonWidget::msg_paint(HDC hdc)  {
    if(m_isButtonPress==true){
		parentWindow->bvm_PaintBitmap(hdc, button_press_bitmap, rc);
    }else{
		parentWindow->bvm_PaintBitmap(hdc, button_lift_bitmap, rc);
    }
//	onEvent_paint(hdc);
	Widget::msg_paint(hdc);
}


void ButtonWidget::setButtonBitmap(int press_bitmap, int lift_bitmap){
	button_press_bitmap = press_bitmap, button_lift_bitmap = lift_bitmap;
}


