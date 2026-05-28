#include "app_sensor.h"

/* 内部辅助函数前向声明 */
int Follow_Route_Mode3(int stop);
int Follow_Route_Mode4(void);

#define Black_CNT1   30
#define Black_CNT2   30
#define Black_CNT3   50
#define White_CNT    1000
#define White_CNT2   5000
#define White_CNT3   7000

extern volatile int flag;
extern volatile bool flag_LED;
extern volatile int mode;

int Follow_Route(void)
{
    static int cnt = 0;

    if (mode == 1) {
        if (flag == 1) {
            if (P1 || P2 || P3 || P4 || P5 || P6 || P7 || P8) {
                cnt++;
                if (cnt > Black_CNT1) {
                    flag_LED = 1;
                    flag = 0;
                    cnt = 0;
                    return 0;
                }
            } else {
                cnt = 0;
            }
        }
    } else if (mode == 2) {
        if (flag == 1) {
            if (P1 || P2 || P3 || P4 || P5 || P6 || P7 || P8) {
                cnt++;
                if (cnt > Black_CNT2) {
                    flag_LED = 1;
                    flag = 2;
                    cnt = 0;
                    return 2;
                }
            } else {
                cnt = 0;
            }
        } else if (flag == 2) {
            if (!(P1 || P2 || P3 || P4 || P5 || P6 || P7 || P8)) {
                cnt++;
                if (cnt > White_CNT2) {
                    flag_LED = 1;
                    flag = 3;
                    cnt = 0;
                    return 3;
                }
            } else {
                cnt = 0;
            }
        } else if (flag == 3) {
            if (P1 || P2 || P3 || P4) {
                cnt++;
                if (cnt > Black_CNT2) {
                    flag_LED = 1;
                    flag = 4;
                    cnt = 0;
                    return 4;
                }
            } else {
                cnt = 0;
            }
        } else if (flag == 4) {
            if (!(P1 || P2 || P3 || P4 || P5 || P6 || P7 || P8)) {
                cnt++;
                if (cnt > White_CNT3) {
                    flag_LED = 1;
                    flag = 0;
                    cnt = 0;
                    return 0;
                }
            } else {
                cnt = 0;
            }
        }
    } else if (mode == 3) {
        Follow_Route_Mode3(0);
        return 3;
    } else if (mode == 4) {
        Follow_Route_Mode4();
        return 4;
    }
    return 0;
}

int Follow_Route_Mode3(int stop)
{
    static int cnt3 = 0;

    if (flag == 1) {
        if (P5 || P6 || P7 || P8) {
            cnt3++;
            if (cnt3 > Black_CNT3) {
                flag_LED = 1;
                flag = 2;
                cnt3 = 0;
                return 2;
            }
        } else {
            cnt3 = 0;
        }
    } else if (flag == 2) {
        if (!(P1 || P2 || P3 || P4 || P5 || P6 || P7 || P8)) {
            cnt3++;
            if (cnt3 > White_CNT3) {
                flag_LED = 1;
                flag = 3;
                cnt3 = 0;
                return 3;
            }
        }
    } else if (flag == 3) {
        if (P1 || P2 || P3 || P4) {
            cnt3++;
            if (cnt3 > Black_CNT3) {
                flag_LED = 1;
                flag = 4;
                cnt3 = 0;
                return 4;
            }
        }
    } else if (flag == 4) {
        if (!(P1 || P2 || P3 || P4 || P5 || P6 || P7 || P8)) {
            cnt3++;
            if (cnt3 > White_CNT3) {
                flag_LED = 1;
                flag = stop;
                cnt3 = 0;
                return 0;
            }
        }
    }
    return 0;
}

int Follow_Route_Mode4(void)
{
    Follow_Route_Mode3(1);
    Follow_Route_Mode3(1);
    Follow_Route_Mode3(1);
    Follow_Route_Mode3(0);
    return 0;
}

int Incremental_Quantity(void)
{
    int value = 0;
    if (!P1) value += 12;
    if (!P2) value += 9;
    if (!P3) value += 7;
    if (!P4) value += 3;
    if (!P5) value -= 3;
    if (!P6) value -= 7;
    if (!P7) value -= 9;
    if (!P8) value -= 12;
    return value;
}

int Incremental_Quantity3(void)
{
    int value = 0;
    if (!P8) value += 9;
    if (!P7) value += 6;
    if (!P6) value += 4;
    if (!P5) value += 2;
    if (!P4) value -= 2;
    if (!P3) value -= 4;
    if (!P2) value -= 6;
    if (!P1) value -= 9;
    return value;
}

int Incremental_Quantity4(void)
{
    int value = 0;
    if (!P1) value += 20;
    if (!P2) value += 10;
    if (!P3) value += 8;
    if (!P4) value += 5;
    if (!P5) value -= 5;
    if (!P6) value -= 8;
    if (!P7) value -= 10;
    if (!P8) value -= 20;
    return value;
}
