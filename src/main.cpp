/*
//  多线程基于FreeRTOS，可以多个任务并行处理；
//  ESP32具有两个32位Tensilica Xtensa LX6微处理器；
//  实际上我们用Arduino进行编程时只使用到了第一个核（大核），第0核并没有使用
//  多线程可以指定在那个核运行；
 */
#include "main.hpp"

void WiFi_Connect()
{
    Serial.print("Connecting.. ");
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{ //这里是阻塞程序，直到连接成功
		delay(300);
		Serial.print(".");
	}
    //WiFi.setAutoReconnect(true); //设置当断开连接的时候自动重连
    //WiFi.persistent(true); //该方法设置将WiFi参数保存于Flash
    Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // String clientId = "ESP8266Client-lcdStation", clientUser = "user8266", clientPasswd = "88888888";
    // String(random(0xffff), HEX)

    // Attempt to connect
    if (client.connect(clientId.c_str(), clientUser.c_str(), clientPasswd.c_str())) {

        //ESP.wdtFeed();

      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(100);
    }
  }
}


void xTask_mqtt_loop(void *xTask_mqtt_loop_p)
{
    while (1)
    {
        client.loop();
        
        // Serial.printf("Taskloop \r\n");
        delay(10);
    }
}


void xTask_mqtt_publish(void *xTask_mqtt_publish_p)
{
    while (1)
    {
        vTaskDelay(2000);
        
    //ets_delay_us(1000);

    // 200 是大小 如果这个Json对象更加复杂，那么就需要根据需要去增加这个值.
  //StaticJsonDocument<200> doc;
 
  // StaticJsonDocument 在栈区分配内存   它也可以被 DynamicJsonDocument（内存在堆区分配） 代替
  DynamicJsonDocument  doc(200);
 
  //添加键值对
  doc["sensor"] = "gps";
  doc["time"] = 1351824120;
 
  // 添加数组.
  JsonArray data = doc.createNestedArray("data");
  data.add(48.756080);
  data.add(2.302038);
 
//串口打印结果
  serializeJson(doc, Serial);
  Serial.println();

//   /**
//  * 构造序列化json，格式化输出
//  * @param doc jsondocument对象
//  * @param output 输出内容
//  */
//   size_t serializeJsonPretty(const JsonDocument& doc, char* output, size_t outputSize);
//   size_t serializeJsonPretty(const JsonDocument& doc, char output[size]);
//   size_t serializeJsonPretty(const JsonDocument& doc, Print& output);
//   size_t serializeJsonPretty(const JsonDocument& doc, String& output);
//   size_t serializeJsonPretty(const JsonDocument& doc, std::string& output);
  
  // string str1 = "ABCDEFG";
  // char a[20];
  // strcpy(a,str1.c_str());//用到 c_str()函数


  String output;
  serializeJson(doc, output);
  Serial.println(output);

  String smsg = output;
	char cmsg[smsg.length()]; 
  strcpy(cmsg,smsg.c_str());

    snprintf (msg, MSG_BUFFER_SIZE, cmsg);
    client.publish("outTopic", msg);
    lastMsg = millis();
    }
}

volatile int servo2switch_flag = 0;
void xTask_servo2switch_node(void *xTask_servo2switch_node_p)
{
    while (1)
    {
    if (servo2switch_flag == 1)
    {
        DynamicJsonDocument  doc(200);
        String output;
        String smsg;
        char cmsg[200];

        //添加键值对
        doc["tx_node"] = "esp32_gateway";
        doc["rx_node"] = "esp8266_servo2switch";
        doc["status_code"] = 123;
        
        // 添加数组.
        
        JsonArray data = doc.createNestedArray("servo_position");
        data.add(90);
        data.add(90);
        data.add(90);

        //串口打印结果

        serializeJson(doc, output);
        Serial.println(output);
        smsg = output;
        strcpy(cmsg,smsg.c_str());
        snprintf (msg, MSG_BUFFER_SIZE, cmsg);
        client.publish("inTopic", msg);
        lastMsg = millis();
        vTaskDelay(3000);

                // 添加数组.
        JsonArray data1 = doc.createNestedArray("servo_position");
        data1.add(180);
        data1.add(180);
        data1.add(180);

        serializeJson(doc, output);
        Serial.println(output);
        smsg = output;
        strcpy(cmsg,smsg.c_str());
        snprintf (msg, MSG_BUFFER_SIZE, cmsg);
        client.publish("inTopic", msg);
        lastMsg = millis();
        vTaskDelay(3000);

        JsonArray data2 = doc.createNestedArray("servo_position");
        data2.add(90);
        data2.add(90);
        data2.add(90);

        //串口打印结果

        serializeJson(doc, output);
        Serial.println(output);
        smsg = output;
        strcpy(cmsg,smsg.c_str());
        snprintf (msg, MSG_BUFFER_SIZE, cmsg);
        client.publish("inTopic", msg);
        lastMsg = millis();
        vTaskDelay(1500);

        servo2switch_flag = 0;
    }
    else;

        vTaskDelay(100);
    }
}


void xTask_tianwen(void *xTask_tianwen_p)
{
    while (1)
    {
        vTaskDelay(1000);
    // read data from serial port
        if(Serial2.available()>0){
            vTaskDelay(100);
            numdata =Serial2.readBytesUntil(character,buffer,3);
            Serial.print("Serial.readBytes:");
            Serial.println(buffer);    
        }
        if(buffer[0] == '0')
        {
            digitalWrite(LED_BUILTIN, LOW);
        }
        else if(buffer[0] == '1')
        {
            digitalWrite(LED_BUILTIN, HIGH);
            servo2switch_flag = 1;
        }
        // clear serial buffer
        while(Serial2.read() >= 0){}
        for(int i=0; i<18; i++){
            buffer[i]='\0';
        }
        
    }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println();
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // String message;
  // for (int i = 0; i < length; i++) {
  //   message = message + (char) payload[i];  // convert *byte to string
  // }
  // Serial.print(message);

  //// Switch on the LED if an 1 was received as first character
  // if ((char)payload[0] == '1') {
  //   digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
  //   // but actually the LED is on; this is because
  //   // it is active low on the ESP-01)
  // } else {
  //   digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  // }

////////////////

    Serial.println("$Received message:");

    String sTopic = topic; //将topic转换为String 可以加以处理

    String sMessage = (char *)payload;        //将消息转换为String
    sMessage = sMessage.substring(0, length); //取合法长度 避免提取到旧消息
    Serial.println(sMessage); //输出消息 用于调试 可以注释掉

    //解析json//
    DynamicJsonDocument jsonBuffer(512); //创建一个DynamicJsonDocument类型的doc对象,大小2048byte
    //DynamicJsonBuffer jsonBuffer;
    //JsonObject& root = jsonBuffer.JsonObject(sMessage);
    Serial.println("从sMessage解码成的DynamicJsonDocument对象doc:");
    deserializeJson(jsonBuffer, sMessage);
    for (int i = 0; i < length; i++) {
      payload[i] = NULL;
    }
    sMessage = "";
    // serializeJson(jsonBuffer, Serial);
    Serial.println("从jsonBuffer对象转换成的JsonObject类型对象root:");
    JsonObject root = jsonBuffer.as<JsonObject>();
    // serializeJson(root, Serial);
    Serial.println();
    Serial.println(jsonBuffer.memoryUsage());
    Serial.println(measureJson(jsonBuffer));
    Serial.println(measureJsonPretty(jsonBuffer));

//-----------------------------------------------判断&赋值-----------------------------------------------//

    String Tx_Node = jsonBuffer["tx_node"];
    String Rx_Node = jsonBuffer["rx_node"];
    String Status_Code = jsonBuffer["status_code"];

    if(Rx_Node == "esp8266_servo2switch"){

    //   for (int index = 0; index < NUM_SERVOS; index++) {
    //     servo_pos[index] = jsonBuffer["servo_position"][index];
    //     Serial.println(servo_pos[index]);
    //   }

    }

    Serial.println(Tx_Node);
    Serial.println(Rx_Node);
    Serial.println(Status_Code);




    // //按字符判断mqtt消息中命令的作用 可以自行定义
    // if (sMessage.charAt(0) == '#')
    // {         //第一位#
    //     if (sMessage.charAt(1) == 'D') 
    //     {     //第一位l
    //         if (sMessage.charAt(2) == '1') 
    //         { //第一位1
    //             //处理#D1
    //             //digitalWrite(LED, 0);  //输出低电平
    //             digitalWrite(LED_BUILTIN, LOW);  
    //         } 
    //         else if (sMessage.charAt(2) == '0') 
    //         {
    //             //处理#D0
    //             //igitalWrite(LED, 1); //初始化输出高电平
    //             digitalWrite(LED_BUILTIN, HIGH);  
    //         }
    //     }
    //     if (sMessage.charAt(1) == 'd') 
    //     {
    //         //处理#d...
    //     }
    // }


}


void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    delay(100);
    while(Serial2.read()>= 0){}//clear serialport
    pinMode(LED_BUILTIN, OUTPUT);

	WiFi_Connect();

    client.setServer(mqtt_server, 1883);
    client.setKeepAlive(65535); //不知道啥东西，保活时间？
    client.setCallback(callback);

    if (!client.connected()) {
        reconnect_mqtt();
    }



}


void loop()
{

#if !USE_MULTCORE

    xTaskCreate(
        xTaskOne,  /* Task function. */
        "TaskOne", /* String with name of task. */
        4096,      /* Stack size in bytes. */
        NULL,      /* Parameter passed as input of the task */
        1,         /* Priority of the task.(configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.) */
        NULL);     /* Task handle. */

    xTaskCreate(
        xTaskTwo,  /* Task function. */
        "TaskTwo", /* String with name of task. */
        4096,      /* Stack size in bytes. */
        NULL,      /* Parameter passed as input of the task */
        2,         /* Priority of the task.(configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.) */
        NULL);     /* Task handle. */

#else

    //最后一个参数至关重要，决定这个任务创建在哪个核上.PRO_CPU 为 0, APP_CPU 为 1,或者 tskNO_AFFINITY 允许任务在两者上运行.
    xTaskCreatePinnedToCore(xTask_mqtt_loop, "Task_mqtt_loop", 4096, NULL, 1, NULL, 0);//mqtt_loop
    xTaskCreatePinnedToCore(xTask_mqtt_publish, "Task_mqtt_publish", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(xTask_tianwen, "Task_tianwen", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(xTask_servo2switch_node, "Task_servo2switch_node", 4096, NULL, 2, NULL, 1);

#endif

    while (1)
    {
        Serial.printf("XTask is running\r\n");
        delay(1000);
    }
}
