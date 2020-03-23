#include <SoftwareSerial.h>;
int data, startByte, numMessage, id, endByte; //return message and message id
String message, container;
SoftwareSerial mySerial(10, 11);

// ####### Commented Serial is just for debugging purpose. Comment it out if in use purpose.

String idbuffer, mode; 

int record;
float pricebuffer, moded_value, return_value;



void mode_computation(){
  String obj_name_buffer, write_buffer;
//  Serial.println("at object_computation");
  if(mode == "p"){
    return_value = moded_value/pricebuffer;
//    Serial.println(return_value);
    obj_name_buffer="eqweight";
    write_buffer=String(return_value,5);
    write_message(&obj_name_buffer, &write_buffer);
//    Serial.println("tatlo here");
    }
  if(mode == "w"){
    return_value = moded_value*pricebuffer;
//    Serial.println(return_value);
    obj_name_buffer="eqprice";
    write_buffer=String(return_value);
    write_message(&obj_name_buffer, &write_buffer);
//    Serial.println("dalwa here");
    }
  }



void catch_values(){
  if(record == 1){
//    Serial.println("at catch_value");
//    Serial.println(id);
    if(id == 12)//0c
    {
      mode = message;
      record = 0;
      }
    if(id == 11)//0b
    {
      pricebuffer = message.toFloat();
      record = 0;
      }
    if(id == 10)//0a
    {
      idbuffer = message;
      record = 0;
      }
     if(id == 13)//0d
     {
      moded_value = message.toFloat();
      record = 0;
      mode_computation();
      }
  
  }
}


void read_message(){

  if (mySerial.available()){ 
      data = mySerial.read();
      
      if (data < 255 && startByte==2){ 
        container.concat(char(data)); 
      }
      
     if (data==255 && startByte==2){ 
       endByte = endByte + 1; 
     }
    
     if (data==255 && endByte==3 && startByte==2){
       message = container; 
       container = ""; 
       numMessage  = 1; 
       endByte = 0;
     }
     if(data < 255 && startByte == 1){
      id = data;
      startByte = 2;
      data = 0;
      }
     
      if(data == 70 && startByte == 0){        
        data = 0;
        startByte = 1;
        }

  }

  if(numMessage==1 && startByte==2){
//    Serial.println("isa here");
//    Serial.println(message);
    numMessage = 0;
    startByte=0;
    record=1;
    catch_values();
    }      
}  



void write_message(String *obj_name, String *str_write){
  mySerial.print(*obj_name);
  mySerial.print(F(".txt=\""));
  mySerial.print(*str_write);
  mySerial.print(F("\""));
  mySerial.write(0xff);
  mySerial.write(0xff);
  mySerial.write(0xff);
//  Serial.println("ok");   
}


void setup() {
  data=0; startByte=0; endByte=0; numMessage=0; id = 0;
  record=0;
  container = "";
//  Serial.begin(115200);
  mySerial.begin(115200);
  while(!mySerial){;}
//  Serial.println("Serial On"); 
  delay(100);

}

void loop() {
 read_message();
}
