#include "main.h"       //반드시 넣을 것!

struct Player {

    int score;
    char name;

}P1, P2, P3, P4;

int KEYPAD[4][4] = { 0 };

static off_t IEB_DOT[MAX_DOT] = {
    IEB_DOT1,
    IEB_DOT2,
    IEB_DOT3,
    IEB_DOT4,
    IEB_DOT5
};

static off_t IEB_FND[MAX_FND] = {
    IEB_FND0,
    IEB_FND1,
    IEB_FND2,
    IEB_FND3,
    IEB_FND4,
    IEB_FND5,
    IEB_FND6,
    IEB_FND7
};

static int fd;
static int map_counter = 0;
static void* map_data[100];

//여기에 키패드 관련 초기설정 추가

int main(int argc, char* argv[]) {

    int i;
    short* led, * dot[MAX_DOT], * fnd[MAX_FND];

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        fprintf(stderr, "Cannot open /dev/mem file");
        exit(EXIT_FAILURE);
    }

    led = mapper(IEB_LED);
    for (i = 0; i < MAX_DOT; i++) {
        dot[i] = mapper(IEB_DOT[i]);
    }
    for (i = 0; i < MAX_FND; i++) {
        fnd[i] = mapper(IEB_FND[i]);
    }

    init_led(led);
    init_dot(dot);
    init_fnd(fnd);
    //초기설정

    if (argc != 5)     //4명 입력이 아닌 경우
        print_clcd('requires', '4 players');

    P1.name = argv[1];
    P2.name = argv[2];
    P3.name = argv[3];
    P4.name = argv[4];

    while (1) {    //게임이 돌아가는 중
        reset_game();
        
        int n_round = 0;  //라운드
        int turn;   //누구의 차례일까요?

        while (1) {   //0라운드부터 시작 : 한명이라도 점수가 0이 되면 반복문 탈출

            turn = n_round % 4;
            print_clcd('ROUND', 'START');    //만약 여기서 출력 문제 있으면 usleep 써보기
            switch (turn) {      //누가 먼저 공격하는지 결정해서 clcd에 출력하기
            case 0: //P1이 공격할 차례 
                print_clcd(P1.name, 'ATTACK');
                if (PRESS(1) == 0)
                    P1.score = P1.score - 1;  // 이상한거 누르면 A 감점   
                if (PRESS(2) == 0)
                    P2.score = P2.score - 1;  // 이상한거 누르면 B 감점
                if (PRESS(3) == 0)
                    P3.score = P3.score - 1;  // 이상한거 누르면 C 감점
                if (PRESS(4) == 0)
                    P4.score = P4.score - 1;  // 이상한거 누르면 D 감점
                clear_dot_matrix();
                compare_data(1);
                break;

            case 1: //P2이 공격할 차례
                print_clcd(P2.name, 'ATTACK');
                if (PRESS(2) == 0)
                    P2.score = P2.score - 1;  // 이상한거 누르면 B 감점
                if (PRESS(3) == 0)
                    P3.score = P3.score - 1;  // 이상한거 누르면 C 감점
                if (PRESS(4) == 0)
                    P4.score = P4.score - 1;  // 이상한거 누르면 D 감점
                if (PRESS(1) == 0)
                    P1.score = P1.score - 1;  // 이상한거 누르면 A 감점
                clear_dot_matrix();
                compare_data(2);
                break;

            case 2: //P3이 공격할 차례
                print_clcd(P3.name, 'ATTACK');
                if (PRESS(3) == 0)
                    P3.score = P3.score - 1;  // 이상한거 누르면 C 감점
                if (PRESS(4) == 0)
                    P4.score = P4.score - 1;  // 이상한거 누르면 D 감점
                if (PRESS(1) == 0)
                    P1.score = P1.score - 1;  // 이상한거 누르면 A 감점
                if (PRESS(2) == 0)
                    P2.score = P2.score - 1;  // 이상한거 누르면 B 감점
                clear_dot_matrix();
                compare_data(3);
                break;

            case 3: //P4이 공격할 차례
                print_clcd(P4.name, 'ATTACK');
                if (PRESS(4) == 0)
                    P4.score = P4.score - 1;  // 이상한거 누르면 D 감점
                if (PRESS(1) == 0)
                    P1.score = P1.score - 1;  // 이상한거 누르면 A 감점
                if (PRESS(2) == 0)
                    P2.score = P2.score - 1;  // 이상한거 누르면 B 감점
                if (PRESS(3) == 0)
                    P3.score = P3.score - 1;  // 이상한거 누르면 C 감점
                clear_dot_matrix();
                compare_data(4);
                break;

            default:
                print_clcd('turn', 'switch error');
                break;
            }

            print_clcd('ROUND', 'END');  //만약 여기서 출력 문제 있으면 usleep 써보기
            show_score();   //앞서 진행한 라운드의 점수 보여주기
            clear_KEYPAD(); //사용된 키패드 데이터 초기화

            if (P1.score == 0 || P2.score == 0 || P3.score == 0 || P4.score == 0)    //한명이라도 점수가 0이면 게임 종료
                break;

            n_round++;  //다음 라운드로 이동
        }

        final();
    }
    unmapper();
    close(fd);
    return 0;
}

void print_clcd(char char1, char char2)
{
    clcd_clear_display(); //clcd 초기화
    clcd_entry_mode_set(1, 0);  //입력 방향 결정
    clcd_set_DDRAM(strlen(char1) - 1);
    clcd_write_string(char1); //clcd 첫째줄 char1 출력

    clcd_set_DDRAM(0x40 + strlen(char2) - 1);
    clcd_write_string(char2); //clcd 둘째줄 char2 출력
}

void reset_game() {      // 키패드 초기화하고 clcd에 시작 문구, fnd에 초기화된 점수 띄우기, 키패드 입력이 될때까지 깜빡이기

    clear_KEYPAD();

    P1.score = 5;   //시작 점수 5로 리셋.
    P2.score = 5;
    P3.score = 5;
    P4.score = 5;

    show_score();
    print_clcd('GAME', 'START');

    clear_KEYPAD();
}

int PRESS(int p) {  //Pi에 대해서 입력 받아오기 : 다른거 눌렀을 경우 0, 제대로 눌렀을 경우 1 반환
    switch (p) {
    case 1: //P1인 경우
        dot_clear();    // dot matrix 초기화
        dot_write(0);    // dot matrix A출력
        break;
    case 2: //P2인 경우
        dot_clear();    // dot matrix 초기화
        dot_write(1);   // dot matrix B출력
        break;
    case 3: //P3인 경우
        dot_clear();    // dot matrix 초기화
        dot_write(2);   // dot matrix C출력
        break;
    case 4: //P4인 경우
        dot_clear();    // dot matrix 초기화
        dot_write(3);   // dot matrix D출력
        break;
    default:
        print_clcd('PRESS', 'ERROR');
        break;
    }
    read_KEYPAD(p);    // 첫번째 눌린 위치 받아오기
    read_KEYPAD(p);    // 두번째 눌린 위치 받아오기

    if (KEYPAD[p - 1][0] + KEYPAD[p - 1][1] + KEYPAD[p - 1][2] + KEYPAD[p - 1][3] == 2)
        return 1;
    else
        return 0;
}

void light_KEYPAD(player) {    //완성
    //keypad에 데이터가 입력되는 순간, LED와 DOT MATRIX 깜빡이기
    // 깜빡임 정도는 각 함수 별로 usleep의 parameter을 조정하면 됨
    // LED 깜빡이기
    led_blink_all();    // 기존 실습에서 썼던 led.h와 led.c에 선언 및 정의
     // DOT MATRIX 깜빡이기
    // dot_inverse()을 dot.c와 dot.h에 정의하여 깃허브에 업로드하겠음
    dot_inverse(4);      //전체 blink
    // 기존 실습에서 제공된 skeleton code(dot.h, dot.c)에 우리 팀이 code을 추가하여 수정한 함수
}

// read_KEYPAD() 선언 및 정의에 따라 기존 실습에서 썼던 keypad.h와 keypad.c 내 keypad_read는 불필요
void read_KEYPAD(int player) { //keypad에 데이터가 입력되는 순간, 입력된 데이터 받아와서 업데이트하기
    int key_value = 0;
    short key_temp;

    for (int col = 0; col < 4; col++) {
        *keypad_out = (short)(0x08 >> col);             // keypad_out과 keypad_in은 keypad.c에 선언 및 정의되어 있음
        key_temp = *keypad_in;

        for (int row = 0; row < 4; row++) {
            if (((key_temp >> row) & 1) == 1)
                key_value = (row * 4) + col;
        }
    }
    light_KEYPAD(player);
    KEYPAD[key_value / 4][key_value % 4] = 1;   // 입력된 키패드의 번호에 대응되는 배열 ON
}

void clear_KEYPAD() {
    memset(KEYPAD, '0', sizeof(KEYPAD)); //keypad 데이터 초기화 : memset 헤더파일 포함
}

void show_score() {
    int score = P1.score * 1000 + P2.score * 100 + P3.score * 10 + P4.score; //총합 스코어 계산

    fnd_hexa_number(score); //fnd 스코어 출력
}

void compare_data(int player_number) //이번 라운드에 잃은 점수 계산
{
    int round_score[4] = { 0 }; //이번 라운드에 잃은 점수
    for (int t = 0; t < 4; t++) {
        for (int k = 0; k < 4; k++) {
            if (KEYPAD[player_number - 1][k] != KEYPAD[t][k])
                round_score[t]++;
        }
    }

    P1.score = P1.score - round_score[0];
    P2.score = P2.score - round_score[1];
    P3.score = P3.score - round_score[2];
    P4.score = P4.score - round_score[3];
}

void final() {    //패배한 사람 찾고, clcd에 띄우기

    char P1_result, P2_result, P3_result, P4_result;

    if (P1.score != 0)
        P1_result = 'WINNER';
    else
        P1_result = 'LOSER';

    if (P2.score != 0)
        P2_result = 'WINNER';
    else
        P2_result = 'LOSER';

    if (P3.score != 0)
        P3_result = 'WINNER';
    else
        P3_result = 'LOSER';

    if (P4.score != 0)
        P4_result = 'WINNER';
    else
        P4_result = 'LOSER';

    print_clcd(P1_result, P1.name);  //usleep?
    print_clcd(P2_result, P2.name);
    print_clcd(P3_result, P3.name);
    print_clcd(P4_result, P4.name);

    print_clcd('PRESS', 'ANY KEY');
    light_KEYPAD();
    clear_KEYPAD();
}

short* mapper(off_t offset) {
    map_data[map_counter] = mmap(NULL, sizeof(short), PROT_WRITE, MAP_SHARED, fd, offset);
    if (map_data[map_counter] == MAP_FAILED) {
        fprintf(stderr, "Cannot do mmap()");
        emergency_closer();
    }
    return (short*)map_data[map_counter++];
}

void unmapper() {
    int i;
    for (i = 0; i < map_counter; i++) {
        munmap(map_data[i], sizeof(short));
    }
    map_counter = 0;
}

void emergency_closer() {
    unmapper();
    close(fd);
    exit(EXIT_FAILURE);
}

void error() {
    dot_error();
    led_clear();
    fnd_clear();
}
