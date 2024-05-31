//-------| src/dot.c |-------//
#include "dot.h"


static unsigned short dot_decimal[10][MAX_DOT] = {
	{0x3f, 0x48, 0x48, 0x48, 0x3F}, // 0 A
	{0x7f, 0x49, 0x49, 0x49, 0x36}, // 1 B
	{0x3e, 0x41, 0x41, 0x41, 0x41}, // 2 C
	{0x7f, 0x41, 0x41, 0x41, 0x3e}, // 3 D
	{0x7f, 0x7f, 0x7f, 0x7f, 0x7f}, // 4 전체 점등
	{0x79, 0x49, 0x49, 0x49, 0x4f}, // 5
	{0x7f, 0x49, 0x49, 0x49, 0x4f}, // 6
	{0x40, 0x40, 0x40, 0x40, 0x7f}, // 7
	{0x7f, 0x49, 0x49, 0x49, 0x7f}, // 8
	{0x78, 0x48, 0x48, 0x48, 0x7f}  // 9
};

static short * dot[MAX_DOT];

/* mmap() 된 주소를 받아오는 함수 */
void init_dot(short * address[]) {
	int i;
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = address[i];	// written_by_JSH_1
	}
}

/* 화면을 지우는 함수 */
void dot_clear() {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = 0;	// written_by_JSH_2
	}
	usleep(0); // for Ximulator
}

/* 화면에 숫자를 그리는 함수 */
void dot_write(int decimal) {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = dot_decimal[decimal][i];	// written_by_JSH_3
	}
	usleep(0); // for Ximulator
}


/* 화면을 0.3초 간격으로 흑백 반전하고 지우는 함수 */
void dot_inverse(int decimal) {
	int shift, i;
	for( shift=0; shift<3; shift++ ) {
		for(i=0; i<MAX_DOT; i++){
			*dot[i] = dot_decimal[decimal][i];
		}
		usleep(300000);
	for(i=0; i<MAX_DOT; i++){
			*dot[i] = ~dot_decimal[decimal][i];		// written_by_JSH_6
		}
		usleep(300000);
	}
	dot_clear();
}
