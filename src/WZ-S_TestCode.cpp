    /*接线
     TX-Rx1
     RX-TX1
     如果没有任何数据返回，请检查接线
     */
    #include <Arduino.h>

    #define DebugSerial Serial
    #define HCHOSerial Serial1
    unsigned char FucCheckSum(unsigned char *i, unsigned char ln);
    byte buffer[9] = {};
    byte Ask_Recever_Moce_Code[9] = {0xFF,0x01,0x78,0x41,0x00,0x00,0x00,0x00,0x46};//切换到问答模式
    byte Auto_Send_Mode_Code[9] = {0xFF,0x01,0x78,0x40,0x00,0x00,0x00,0x00,0x47};//切换到主动上传模式
    byte Ask_code[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};//问答模式下，请求发送
    int count = 0;
    bool bufferComplete = false;

    void get_HCHO();

    void setup() {
      DebugSerial.begin(115200);
      HCHOSerial.begin(9600, SERIAL_8N1, 6, 5);
      
    //   for(int i=0;i<9;i++){
    // HCHOSerial.write(Ask_Recever_Moce_Code[i]);
    // //HCHOSerial.write(Auto_Send_Mode_Code[i]);//根据需要打开注释切换模式
    //   }
      
    }
    
    void loop() {
    //   for(int i=0;i<9;i++){
    // HCHOSerial.write(Ask_code[i]);//在问答模式下请求数据
    //   }  
      get_HCHO();
      int h1 = (int)buffer[2]*256+(int)buffer[3];
      float h2 = ((int)buffer[4]*256+(int)buffer[5])/(int)buffer[2]/1000.0;
      DebugSerial.print(h1);
      DebugSerial.print(" ");
      DebugSerial.print("HCHO: ");
      DebugSerial.print(h2, 5);
      DebugSerial.println(" Ppm");
      bufferComplete = false;
      delay(1000);
    }
    
    void get_HCHO(){
        int flag_end = false;
        int flag_start = false;
        while (flag_end == false)
        {
            if (HCHOSerial.available() > 0) {//接收到数据
                byte inChar = HCHOSerial.read();
                buffer[count] =inChar;
                if (buffer[count] == 0xFF){//接收到起始标志
                    count = 0;  
                    flag_start = true;
                    DebugSerial.println("\r\n##################################################");
                    DebugSerial.println("start");
                }
                count++;
                if (count >=9){//接收9个byte数据
                    count = 0;
                    if (flag_start){
                        for (int i = 0 ; i < 9 ; i++){
                            DebugSerial.print(buffer[i], HEX);//以16进制输出接收到的数据
                            DebugSerial.print(",");
                        }
                        DebugSerial.println("");
                        if(buffer[8]==FucCheckSum(buffer,9)){ //校验  
                            flag_end=true;
                        }
                    }
                }
            }
        }
    }
unsigned char FucCheckSum(unsigned char *i, unsigned char ln){
    unsigned char j, tempq=0; i+=1;
    for(j=0; j<(ln-2); j++){
        tempq+=*i;
        i++; 
    }
    tempq=(~tempq)+1;
    return(tempq);
}