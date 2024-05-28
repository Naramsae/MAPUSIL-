//-------| src/main.c |-------//
#include "main.h"


static off_t IEB_DOT[MAX_DOT] = {
	IEB_DOT1,
	IEB_DOT2,
	IEB_DOT3,
	IEB_DOT4,
	IEB_DOT5
};

static int fd;
static int map_counter = 0;
static void * map_data[100];


int main() {
	
	int i;
	short * led, * dot[MAX_DOT];
	
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		fprintf(stderr, "Cannot open /dev/mem file");
		exit(EXIT_FAILURE);
	}
	
	led = mapper(IEB_LED);
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = mapper(IEB_DOT[i]);
	}
	init_led(led);
	init_dot(dot);

	
	while( inputter() == TRUE ) {	}
	
	unmapper();
	close(fd);
	return 0;
}

short * mapper(off_t offset) {
	map_data[map_counter] = mmap(NULL, sizeof(short), PROT_WRITE, MAP_SHARED, fd, offset);
	if ( map_data[map_counter] == MAP_FAILED ) {
		fprintf(stderr, "Cannot do mmap()");
		emergency_closer();
	}
	return (short *)map_data[map_counter++];
}

void unmapper() {
	int i;
	for( i=0; i<map_counter; i++) {
		munmap(map_data[i], sizeof(short));
	}
	map_counter = 0;
}

void emergency_closer() {
	unmapper();
	close(fd);
	exit(EXIT_FAILURE);
}

truth_t inputter() {
	int input_int;

	printf("\n");
	printf("**********************************************\n");
	printf("*     Please type a number (0~9) + mode      *\n");
	printf("*  normal : + 0 ( 0~ 9)  up   : +10 (10~19)  *\n");
	printf("*  down   : +20 (20~29)  exit : +40 (40~49)  *\n");
	printf("*     ( e.g. 28 is '8' with 'up' mode )      *\n");
	printf("**********************************************\n\n");

	scanf("%d", &input_int);
	
	
	if( ( input_int / 10 ) == 4 ) {
		return FALSE;
	}

	if( ( input_int / 10 ) == 1 ) {
		led_up_shift();
		dot_up_shift(input_int % 10);
	}
	else if( ( input_int / 10 ) == 2 ) {
		led_down_shift();
		dot_down_shift(input_int % 10);
	}
	else if( ( input_int / 10 ) == 3 ) {
		led_blink_all();
		dot_inverse(input_int % 10);
	}
	else {
		dot_write(input_int % 10);
		usleep(1000000);
		dot_clear();
	}

	
	return TRUE;
}
