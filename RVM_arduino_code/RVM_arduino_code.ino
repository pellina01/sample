//
// *********************************************************************** Rice Vending Machine Documentation *********************************************************************
//
//
// data structure to be extracted (Read message from nextion):
//     --------------------------------------------------------------------------------------
//    |  start  |          |                            |    end   |    end     |    end     |
//    |   byte  |    ID    |      message               |    byte  |    byte    |    byte    |
//    |         |          |                            |          |            |            |
//     --------------------------------------------------------------------------------------
//    
//  start byte - start bit. Para madistinguish ng arduino kung sudden impulse/noise lang yung narecieve ng RX nya. 
//              Pag 70(70 in decimal/46 hex value sa nextion) narecieve nya, automatic start bit yon, tapos mag kacount yung start bit variable
//
//  ID - para madistinguish kung para saan yung marerecieve na data. Isesave ang ID sa ID variable para malaman sa catch_value() function kung para saan ang message
//
//  message - data/values na gagamiting
//
//  end byte - para malaman ng arduino na tapos na yung message. 3 end bytes na may 255(FF sa hex) ang isesend ng nextion sa arduino for confirmation
//  
//  Lahat ng process na ito ay ginagawa ni read_message() function
//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//
// data structure to send (Write message to nextion):
//     ---------------------------------------------------------------------------------------
//    |  *obj   |          |              |              |    end   |    end     |    end     |
//    |  name   |  .txt\"  |   *str_write |    \"        |    byte  |    byte    |    byte    |
//    |         |          |              |              |          |            |            |
//     ---------------------------------------------------------------------------------------
//    
//  Note: dapat lagi naka global ang object/variable sa nextion para madaling iaccess sa arduino. dapat din ay unique ang object name nito at STRING lagi ang data type.
//        Hindi na din kailangan ng nextion makarecieve ng start byte, kaya diretso agad sa object name
//
//  obj_name - kung anong object ang dapat makatanggap ng message(text box, variable, button, etc) sa nextion. Dapat unique ang object name at naka global para madaling iaccess.
//             Dapat naka pointer type din ito dahit sa C++, hindi pwede magpasa ng string parameter sa isang function kaya, ang solusyon ay mag assign ng pointer sa isang string
//             at ang pointer ang ipasa sa function. Kaya kapag gagamitin ang function na write_message()
//             dapat ay magpasa nalang ng pointer( may ampersand & na variable)
//
//  .txt\" -    laging string ang data type ng mga obj sa nextion. Wala kasing float sa nextion hindi ito supported kaya kung kailangan magdisplay
//              nang values na may decimal point ay dapat naka string. ang purpose ng \" ay para maglagay ng apostrophy(") sa message(*str_write)
//
//  *str_write - eto yung string message na ipapadala natin sa nextion na pointer type din katulad ng *obj_name(object name). 
//
//        \"  -  para ienclose ang message(*str_write) sa apostrophy(")
//
//  endbyte   - para malaman ng nextion na tapos na ang message. 3 endbyte na 255(FF sa hex) ang value ang dapat ipadala
//
//  ang buong process ay ginagawa ni write_message() function
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Catch_value Fuction explanation:
//
// catch_value() Function - tagachech kung para saan ang message, tinitignan yung values ng ID tapos isostore sa isang global variables
//                        example: ID = 0a, then IDbuffer = message -> ibig sabihin kapag a0 yung value ng ID ng data packet, yung laman ng message ay yung id kung
//                                                                     aling rice ang nagkakaroon ng transaction o ididispense(rice1, rice2, rice3);
//
//                                 ID = 0b, then pricebuffer = message -> kapag 0b ang ID ng data packet, eto yung price ng rice kada kilo;
//
//                                 ID = 0c, then mode = message -> kapag 0c ang ID ng data packet, eto yung mode ng computation. 
//                                                                 kapag "w", per weight ang computation.
//                                                                 makakarecieve tayo ng weight tapos magrereturn tayo ng price kung magkano yung kilo na yun;
//                                                                 kapag "p", per price ang computation.
//                                                                 makakarecieve tayo ng price tapos magrereturn tayo ng weight kung ilan ba ang katumbas na kilo ng presyo na 
//                                                                 ibinigay sa atin
//                                 ID = 0d, then moded_value = message -> eto yung ibinigay sa atin na value, kapag "w" ang mode,
//                                                                        ibig sabihin kilo ang binigay sa atin(value ng moded_value);
//                                                                        kapag "p" ang mode, ibig sabihin presyo ang ibinigay sa atin(value ng moded_value)
//                                                                     -> kapag narecieve na natin ang moded_value, didiretso tayo sa mode_computation() function
//                                                                        kung saan gagawin ang computation depende sa mode na narecieve.
//
//
//
//
//
//
//
// ******************************************************************************  END  ******************************************************************************************





// Note: Commented Serial is just for debugging purpose. Comment it out if in use.




#include <SoftwareSerial.h>;
int data, startByte, numMessage, id, endByte; //return message and message id
String message, container;
SoftwareSerial mySerial(10, 11);

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
