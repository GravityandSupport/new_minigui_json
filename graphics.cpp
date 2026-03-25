#include "graphics.hpp"


Grap::RGB::RGB(int _r, int _g, int _b):r(_r),g(_g),b(_b){

}

Grap::RGB::RGB(uint32_t rgb_hex){
	rgb_hex &= 0xffffff;
	this->r = (rgb_hex >> 16) & 0xff;
    this->g = (rgb_hex >> 8) & 0xff;
    this->b = rgb_hex & 0xff;
}

gal_pixel Grap::RGB::toPixel(HDC hdc) const {
	return RGB2Pixel(hdc, r, g, b);
}
Grap::RGB Grap::RGB::fromPixel(HDC hdc, gal_pixel pixel){
	uint8_t r,g,b;
	Pixel2RGB(hdc, pixel, &r, &g, &b);
	return RGB(r, g, b);
}
Grap::Rect::Rect(int _x, int _y, int _w, int _h):x(_x), y(_y), w(_w), h(_h){
	SetRect(&rc, x, y, x+w, y+h);
}
Grap::Rect::Rect(const RECT& _rc){
	x = _rc.left;
	y = _rc.top;
	w = RECTW(_rc);
	h = RECTH(_rc);
	CopyRect(&rc, &_rc);
}

const RECT& Grap::Rect::toRECT() const {
	return rc;
}

void Grap::rectangle(HDC hdc, const Rect& rc, const RGB& pen_color, int pen_width){
	pen_width = pen_width + (pen_width&1); // 向上取偶数
	SetPenColor(hdc, pen_color.toPixel(hdc));
	SetPenWidth(hdc,pen_width);
	RECT _rc = rc.toRECT();
	int half_width = pen_width/2;
	LineEx(hdc,_rc.left, _rc.top+half_width, _rc.right, _rc.top+half_width);
	LineEx(hdc,_rc.right-half_width, _rc.top, _rc.right-half_width, _rc.bottom);
	LineEx(hdc,_rc.right, _rc.bottom-half_width, _rc.left, _rc.bottom-half_width);
	LineEx(hdc,_rc.left+half_width, _rc.bottom, _rc.left+half_width, _rc.top);
}

void Grap::roundRect(HDC hdc, const Rect& rc, int rw, int rh, const RGB& brush_color, const RGB& pen_color, int pen_width){
	SetBrushColor(hdc, brush_color.toPixel(hdc));
	pen_width = pen_width + (pen_width&1); // 向上取偶数
	SetPenColor(hdc, pen_color.toPixel(hdc));
	SetPenWidth(hdc,pen_width);
	RECT _rc = rc.toRECT();
	int half_width = pen_width/2;
	RoundRect(hdc, _rc.left+half_width, _rc.top+half_width, _rc.right-half_width, _rc.bottom-half_width, rw, rh);
}

void Grap::fillBox(HDC hdc, const Rect& rc, const RGB& brush_color){
	SetBrushColor(hdc, brush_color.toPixel(hdc));
	RECT _rc = rc.toRECT();
	FillBox(hdc, _rc.left, _rc.top, RECTW(_rc), RECTH(_rc));
}

