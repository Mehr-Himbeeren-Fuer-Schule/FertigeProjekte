
#include <wiringPi.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <ctime>

typedef unsigned char uchar;
typedef unsigned int uint;

const int     ADC_CS  = 0;
const int     ADC_CLK = 1;
const int     ADC_DIO = 2;


uchar get_ADC_Result(void) {
	uchar i;
	uchar dat1=0, dat2=0;

	digitalWrite(ADC_CS, 0);
	digitalWrite(ADC_CLK,0);
	digitalWrite(ADC_DIO,1);	delayMicroseconds(2);
	digitalWrite(ADC_CLK,1);	delayMicroseconds(2);

	digitalWrite(ADC_CLK,0);
	digitalWrite(ADC_DIO,1);    delayMicroseconds(2);
	digitalWrite(ADC_CLK,1);	delayMicroseconds(2);

	digitalWrite(ADC_CLK,0);
	digitalWrite(ADC_DIO,0);	delayMicroseconds(2);
	digitalWrite(ADC_CLK,1);
	digitalWrite(ADC_DIO,1);    delayMicroseconds(2);
	digitalWrite(ADC_CLK,0);
	digitalWrite(ADC_DIO,1);    delayMicroseconds(2);

	for(i=0;i<8;++i)
	{
		digitalWrite(ADC_CLK,1);	delayMicroseconds(2);
		digitalWrite(ADC_CLK,0);    delayMicroseconds(2);

		pinMode(ADC_DIO, INPUT);
		dat1=dat1 << 1 | digitalRead(ADC_DIO);
	}

	for(i=0;i<8;++i)
	{
		dat2 = dat2 | ((uchar)(digitalRead(ADC_DIO))<<i);
		digitalWrite(ADC_CLK,1); 	delayMicroseconds(2);
		digitalWrite(ADC_CLK,0);    delayMicroseconds(2);
	}

	digitalWrite(ADC_CS,1);

	return(dat1==dat2) ? dat1 : 0;
}

int run(void) {
    std::fstream fs;
    std::string adress;

    adress = "temp_data.js";
    fs.open(&adress[0], std::ios::app);
    fs << "temp_data.length++;" << std::endl;
    fs << "temp_data[temp_data.length -1] = {";
    time_t now = time(0);

    //Temperatur auslesen:
	uchar analogVal;
	double Vr, Rt, temp;

	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
		std::cout <<"WIRING PI SETUP FAILED";
		return 1;
	}

	pinMode(ADC_CS,  OUTPUT);
	pinMode(ADC_CLK, OUTPUT);

    std::string time = std::ctime(&now);
    time.replace(time.length() -1,time.length() ,"");
    fs << "0:'" << time << "',1:[";
    int iteration = 0;
    int i_max = 100;
	while(1){


		pinMode(ADC_DIO, OUTPUT);

		analogVal = get_ADC_Result();//Kelvin
		//Beispiel:Klevin -> Celsius
		Vr = 5 * (double)(analogVal) / 255;
		Rt = 10000 * (double)(Vr) / (5 - (double)(Vr));
		temp = 1 / (((log(Rt/10000)) / 3950)+(1 / (273.15 + 25)));
		temp = temp-237.15;//temp=> °C
        //In Datei schreiben.

        if(iteration > 0) {
            fs << "," << temp;
        } else {
            fs << temp;
        }
        std::cout << "Temp:" << temp <<"\t"<< iteration <<"\n";
        if(iteration >= i_max) {
            std::cout << "Noch einmal x100 lesen?\n";
            std::string ans;
            std::cin >> ans;
            if(ans == "J")
                iteration = 0;
            else
                break;
        }
        iteration += 1;
        delayMicroseconds(60*60*60); //60ms*60*60 = 1 min.
	}
	fs << "]};";
	fs.close();

	return 0;
}

int main(void) {
    run();
}
