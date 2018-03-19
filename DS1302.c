#include "DS1302.h"

#define SETTING_KEY   P3_2
#define OPERATION_KEY P3_3

#define  DT_SS        P1_7    // 段选
#define  DT_DATA      P2
unsigned char CC_LED_DATA[] = { 0xF9, 0x60, 0xDA, 0xF2, 0x63, 0xB3, 0xBB, 0xE0, 0xFB, 0xF3, 0x00 };

ds1302_Datetime datetime;

unsigned char lcd_display[8];
__bit OPERATION_KEY_PRESSED = false;
__bit SETTING_KEY_PRESSED = false;

/**
 * 设置模式
 * 0 关闭状态
 * 1 时设置
 * 2 分设置
 *
 * increase 标识用户是否按下调整按键
 */
unsigned char SETTING_MODE = 0, increase=0, twinkle = 0;

void delay(unsigned int millisecond)
{
  unsigned char a,b;
  while(millisecond--)
  {
    for(b=102;b>0;b--)
      for(a=3;a>0;a--);
  }
}

void setup()
{
  SETTING_KEY = high;
  OPERATION_KEY = high;
}

void loop()
{
  ds1302_get_datetime(&datetime);

  // 显示模式切换按键
  if(OPERATION_KEY == low && !OPERATION_KEY_PRESSED)
  {
    OPERATION_KEY_PRESSED = true;
  }
  else if(OPERATION_KEY == high && OPERATION_KEY_PRESSED)
  {
    OPERATION_KEY_PRESSED = false;
    increase=1;
  }

  // 设置模式切换按键
  if(SETTING_KEY == low && !SETTING_KEY_PRESSED)
  {
    SETTING_KEY_PRESSED = true;
  }
  else if(SETTING_KEY == high && SETTING_KEY_PRESSED)
  {
    SETTING_KEY_PRESSED = false;
    SETTING_MODE++;
    if(SETTING_MODE > 2) SETTING_MODE = 0;
  }

  // 设置
  if(SETTING_MODE && increase)
  {
    switch (SETTING_MODE) {
      case 1:
      {
        datetime.hour++;
        if(datetime.hour > 23) datetime.hour = 0;
        break;
      }
      case 2:
      {
        datetime.minute++;
        if(datetime.minute > 59) datetime.minute = 0;
        break;
      }
      default:
        break;
    }
    increase = 0;
    datetime.second = 0;
    ds1302_set_datetime(&datetime);
  }

  if(SETTING_MODE == 1 && twinkle > 40)
  {
    lcd_display[0] = 10;
    lcd_display[1] = 10;
  }
  else
  {
    lcd_display[0] = datetime.hour / 10;
    lcd_display[1] = datetime.hour % 10;
  }
  if(SETTING_MODE == 2 && twinkle > 40)
  {
    lcd_display[2] = 10;
    lcd_display[3] = 10;
  }
  else
  {
    lcd_display[2] = datetime.minute / 10;
    lcd_display[3] = datetime.minute % 10;
  }

  P0_0 = 0;
  P0_1 = 1;
  P0_2 = 1;
  P0_3 = 1;

  DT_SS = 1;
  DT_DATA = CC_LED_DATA[lcd_display[0]];
  DT_SS = 0;
  delay(1);

  P0_0 = 1;
  P0_1 = 0;
  P0_2 = 1;
  P0_3 = 1;

  DT_SS = 1;
  DT_DATA = CC_LED_DATA[lcd_display[1]];
  DT_SS = 0;
  delay(1);

  P0_0 = 1;
  P0_1 = 1;
  P0_2 = 0;
  P0_3 = 1;

  DT_SS = 1;
  DT_DATA = CC_LED_DATA[lcd_display[2]];
  DT_SS = 0;
  delay(1);

  P0_0 = 1;
  P0_1 = 1;
  P0_2 = 1;
  P0_3 = 0;

  DT_SS = 1;
  DT_DATA = CC_LED_DATA[lcd_display[3]];
  DT_SS = 0;
  delay(1);

  P0_4 = 0;

  twinkle = twinkle > 80 ? 0 : ++twinkle;
}
