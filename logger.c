#include "logger.h"



//void logger(log_level level,function_type function)
//{
//
//
//	void blink(int LED_no, int delay_counter);
//	void Handle_Temp_Read();
//	void Handle_Average_Wait();
//	void Handle_Temperature_Alert();
//	void Handle_Disconnect();
//	void POST();
//	void state_machines();
//	void i2c_write_byte(uint8_t dev, uint8_t reg, uint8_t data_byte1,uint8_t data_byte2);
//	int i2c_read_bytes(uint8_t dev_adx,uint8_t reg_adx,int CR);
//	switch(level)
//	{
//	case(status):{switch(function){
//
//	case(blink_led):{}
//	case(Handle_Temperature_Read):{}
//	case(Handle_Avg_Wait):{}
//	case(Handle_Temp_Alert):{}
//	case(Handle_Disc):{}
//	case(Power_start):{}
//	case(state_machines):{}
//	case(i2c_write):{}
//	case(i2c_read):{}
//	};
//	break;}
//	}
//
//	case(test):{switch(function){case():{}
//
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	}break;}
//
//	case(debug):{switch(function){case():{}
//
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	case():{}
//	}break;}
//	}
//}
void log_i2c_read_bytes(int config,int data_buf[3])
{

		if (config==1)
	        PRINTF("\n\r%d",data_buf[0]);

		else
	        PRINTF("\n\r%d\n\r",data_buf[1]);

}

void log_i2c_write_bytes()
{
	PRINTF("\n\rWRITE OPERATION\n\r");
}

void log_blink_led(int check_led)
{
	if (check_led==1)
	PRINTF("\n\rBLINK RBG LEDS\n\r\n\r");
//	else if (check_led==2)
//	PRINTF("\n\rBLINK  LED\n\r\n\r");)
}
void log_Handle_Temp_Read(int temperature_read)
{
	PRINTF("\n\r TEMPERATURE READ=%d",temperature_read);
}
void log_Handle_Average_Wait(int8_t current_data,int8_t AVERAGE)
{
	    PRINTF("\n\rCURRENT TEMPERATURE=%d\n\r", current_data);
	    PRINTF("\n\rAVERAGE TEMPERATURE=%d\n\r",AVERAGE);

}
void log_Handle_Temperature_Alert()
{
	PRINTF("\n\rALERT STATE\n\r");
}

void log_Handle_Disconnect()
{
	PRINTF("\n\n\r--------------------------------- Sensor Disconnected-----------------------\n\n\r ");
}

void log_POST(int check_post)
{
	if (check_post==1)
	PRINTF("\n\n\r-------------------------Power on Self test--------------------------------------------\n\r");
	else
	PRINTF("\n\n \n\r-------------------POST Successful----------------------------------------------\n\r");
}


void log_state_machines(int check_state_machine)
{
//	//int cases;
	switch(check_state_machine)
	{
	case(0):{PRINTF("\n\r State Machine 1\n\r");break;}
	case(1):{PRINTF("\n\r The sensor is in TEMP_READ STATE\n\r");break;}
	case(2):{PRINTF("\n\r The sensor is in AVERAGE_WAIT STATE\n\r");break;}
	case(3):{PRINTF("\n\r The sensor is in TEMP_ALERT STATE\n\r");break;}
	case(4):{PRINTF("\n \n\r State Machine 2\n\r");break;}
	case(5):{PRINTF("\n \n\rWe are in Temperature read state\n\r");break;}
	case(6):{PRINTF("\n\n\r We are in Temperature Wait Average state\n\r");break;}
	case(7):{PRINTF("\n\r We are in Temperature Alert state\n\r");break;}

	}
}





void log_next_line()
{
	PRINTF("\n\r");
}
