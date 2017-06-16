#include "ioDefinition.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <DS1302.h>
#include <Adafruit_ssd1306syp.h>
#include "charBitmaps.h"

Adafruit_ssd1306syp oled(_IO_OLED_SDA,_IO_OLED_SCL);
DS1302 rtc(_IO_CLOCK_RST,_IO_CLOCK_DAT,_IO_CLOCK_CLK);

/* 日期变量缓存 */
char buf[50];
char day[10];
boolean stopFlag,huaji;
time_t lastTime;

time_t makeTime(int year,int month,int day,int hour,int minute,int second){
	struct tm timeconv;
	timeconv.tm_hour=hour;
	timeconv.tm_min=minute;
	timeconv.tm_sec=second;
	timeconv.tm_year=year;
	timeconv.tm_mon=month;
	timeconv.tm_mday=day;
	timeconv.tm_isdst=0;
	return(mktime(&timeconv));
}

void zhuangbiLED(long last){
	long total=0;
	randomSeed(analogRead(5));
	while(total<last){
		int thistime;

		thistime=random(233);
		total+=thistime;
		digitalWrite(_IO_TO_BOARD_LED,HIGH);
		delay(thistime);

		thistime=random(233);
		total+=thistime;
		digitalWrite(_IO_TO_BOARD_LED,LOW);
		delay(thistime);
	}
	digitalWrite(_IO_TO_BOARD_LED,LOW);
}

void showBoot(){
	oled.drawBitmap(0,0,_CHARBMP_chinese_tian1,16,16,1);
	oled.drawBitmap(16,0,_CHARBMP_chinese_hai1,16,16,1);
	oled.drawBitmap(32,0,_CHARBMP_chinese_xin1,16,16,1);
	oled.drawBitmap(48,0,_CHARBMP_chinese_xi1,16,16,1);
	oled.drawBitmap(64,0,_CHARBMP_chinese_ke1,16,16,1);
	oled.drawBitmap(80,0,_CHARBMP_chinese_ji1,16,16,1);
	oled.setCursor(0,17);
	oled.println("http://tianhai.info");
	oled.setCursor(0,25);
	oled.println("Booting...");
	oled.update();
	zhuangbiLED(876);
	delay(475);
	oled.setCursor(0,33);
	zhuangbiLED(100);
	delay(20);
	oled.println("Author: Soha King");
	oled.update();
	zhuangbiLED(666);
	delay(20);
	oled.setCursor(0,41);
	oled.println("Initializaing...");
	oled.update();
	zhuangbiLED(6666);
	delay(20);
}

void shutDownScreen(){
	oled.clear();
	oled.update();
}

time_t getTime(){
	Time timeNow=rtc.time();
	struct tm timeconv;
	timeconv.tm_hour=timeNow.hr;
	timeconv.tm_min=timeNow.min;
	timeconv.tm_sec=timeNow.sec;
	timeconv.tm_year=timeNow.yr;
	timeconv.tm_mon=timeNow.mon;
	timeconv.tm_mday=timeNow.date;
	timeconv.tm_isdst=0;
	return(mktime(&timeconv));
}

void prepareCountdownInfo(){
	long cdtime=(difftime(makeTime(2018,6,7,9,0,0),getTime()));
	int day=cdtime/86400;
	cdtime=cdtime%86400;
	int hour=cdtime/3600;
	cdtime=cdtime%3600;
	int minute=cdtime/60;
	cdtime=cdtime%60;
	int second=cdtime;

	char days[10],hr[10],mins[10],secs[10];
	dtostrf((double)(day),4,0,days);
	dtostrf((double)(hour),2,0,hr);
	dtostrf((double)(minute),2,0,mins);
	dtostrf((double)(second),2,0,secs);
	displayNumber(days,0,48);
	oled.drawBitmap(32,48,_CHARBMP_chinese_tian1,16,16,1);
	displayNumber(hr,56,48);
	oled.drawBitmap(72,48,_CHARBMP_symbol_colon_half,8,16,1);
	displayNumber(mins,80,48);
	oled.drawBitmap(96,48,_CHARBMP_symbol_colon_half,8,16,1);
	displayNumber(secs,104,48);
}

void prepareDateTime(){
	char yr[10],mth[10],days[10],hr[10],mins[10],secs[10],dow[1];
	Time timeNow=rtc.time();
	dtostrf((double)(timeNow.yr),4,0,yr);
	dtostrf((double)(timeNow.mon),2,0,mth);
	dtostrf((double)(timeNow.date),2,0,days);
	dtostrf((double)(timeNow.hr),2,0,hr);
	dtostrf((double)(timeNow.min),2,0,mins);
	dtostrf((double)(timeNow.sec),2,0,secs);
	dtostrf((double)((timeNow.day)-1),1,0,dow);
	displayNumber(yr,0,0);
	oled.drawBitmap(32,0,_CHARBMP_chinese_nian1,16,16,1);
	displayNumber(mth,48,0);
	oled.drawBitmap(64,0,_CHARBMP_chinese_yue1,16,16,1);
	displayNumber(days,80,0);
	oled.drawBitmap(96,0,_CHARBMP_chinese_ri1,16,16,1);
	
	displayChineseNumber(dow,0,16);

	displayNumber(hr,32,16);
	oled.drawBitmap(48,16,_CHARBMP_symbol_colon_half,8,16,1);
	displayNumber(mins,56,16);
	oled.drawBitmap(72,16,_CHARBMP_symbol_colon_half,8,16,1);
	displayNumber(secs,80,16);
}

int numberCharToInt(char o) {
	if(o=='0')return(0);
	if(o=='1')return(1);
	if(o=='2')return(2);
	if(o=='3')return(3);
	if(o=='4')return(4);
	if(o=='5')return(5);
	if(o=='6')return(6);
	if(o=='7')return(7);
	if(o=='8')return(8);
	if(o=='9')return(9);
	return(0);
}

void displayNumber(char *e, int x, int y) {
	int len=strlen(e);
	int lop;
	for(lop=0;lop<len;lop++){
		if((char *)e[lop]!=(char *)'.'){
			oled.drawBitmap(x+lop*8,y,_CHARBMP_numbers[numberCharToInt(e[lop])],8,16,1);
		}else{
			oled.drawBitmap(x+lop*8,y,_CHARBMP_symbol_dot,8,16,1);
		}
	}
}

void displayChineseNumber(char *e, int x, int y) {
	int len=strlen(e);
	int lop;
	for(lop=0;lop<len;lop++){
		if((char *)e[lop]!=(char *)'.'){
			oled.drawBitmap(x+lop*16,y,_CHARBMP_chinese_numbers[numberCharToInt(e[lop])],16,16,1);
		}/*else{
			oled.drawBitmap(x+lop*16,y,_CHARBMP_symbol_space,16,16,1);
		}*/
	}
}

void displayChineseDOW(char *e, int x, int y) {
	int len=strlen(e);
	int lop;
	for(lop=0;lop<len;lop++){
		if((char *)e[lop]!=(char *)'.'){
			int swap=numberCharToInt(e[lop]);
			if(swap==0){
				oled.drawBitmap(x+lop*16,y,_CHARBMP_chinese_ri1,16,16,1);
			}else{
				oled.drawBitmap(x+lop*16,y,_CHARBMP_chinese_numbers[swap],16,16,1);
			}
		}/*else{
			oled.drawBitmap(x+lop*16,y,_CHARBMP_symbol_space,16,16,1);
		}*/
	}
}

void setup(){
	Serial.begin(9600);

	pinMode(_IO_TO_BOARD_LED,OUTPUT);//板载LED初始化
	rtc.writeProtect(true);//时钟模块初始化
	rtc.halt(false);//时钟模块初始化
	oled.initialize();//OLED初始化
	oled.clear();//OLED初始化
	showBoot();
	shutDownScreen();
	
	oled.setCursor(0,33);
	oled.println(" - plz press the button to see.");
	oled.update();
	delay(2000);
	shutDownScreen();
}

void loop(){
	Time ttttt=rtc.time();

	if(analogRead(_IO_BUTTON_AP)>700){
		lastTime=makeTime(ttttt.yr,ttttt.mon,ttttt.date,ttttt.hr,ttttt.min,ttttt.sec);
	}

	if(difftime(getTime(),lastTime)<=10){
		oled.clear();
		prepareDateTime();
		prepareCountdownInfo();
		displayEvent();
		for(int i=0;i<128;i+=2){
			oled.drawPixel(i,31,1);
		}
		oled.update();
		digitalWrite(_IO_TO_BOARD_LED,(huaji ? HIGH : LOW));
		huaji=!huaji;
	}else{
		huaji=false;
		shutDownScreen();
		digitalWrite(_IO_TO_BOARD_LED,LOW);
	}
	delay(40);
}

void displayEvent(){
	displayNumber("2018",8,32);
	oled.drawBitmap(40,32,_CHARBMP_chinese_nian1,16,16,1);
	oled.drawBitmap(56,32,_CHARBMP_chinese_gao1,16,16,1);
	oled.drawBitmap(72,32,_CHARBMP_chinese_kao1,16,16,1);
	oled.drawBitmap(88,32,_CHARBMP_symbol_colon,8,16,1);
}