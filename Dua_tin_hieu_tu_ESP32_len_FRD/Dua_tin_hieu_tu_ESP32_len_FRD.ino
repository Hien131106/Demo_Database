#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h" //Trình trợ giúp mã thông báo để tạo mã thông báo
#include "addons/RTDBHelper.h" //Trình trợ giúp RTDB hỗ trợ tải trọng cơ sở dữ liệu thời gian thực

#define WIFI_SSID "1️⃣0️⃣0️⃣9️⃣"
#define WIFI_PASSWORD "one00nine"
#define API_KEY "AIzaSyBWT57BTZmAHL_zvMeygR5necWtomlmWFQ"
#define DATABASE_URL "https://fir-esp32-a44d8-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define LED 19
#define POT 34

FirebaseData fbdo; // Xử lí dữ liệu khi dữ liệu thay đổi ở đường dẫn cơ sở dữ liệu
FirebaseAuth auth; // Xác thực
FirebaseConfig config; // Cấu hình

unsigned long sendDataPrevMillis = 0; // Biến dùng để đọc và ghi dữ liệu theo 1 khoảng thời gian xác định
bool signupOK = false;
int POTData = 0;
float voltage = 0.0;

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println();
  Serial.print("Connected with IP:");
  Serial.print(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY; //Chỉ định khóa API trong cấu hình Firebase
  config.database_url = DATABASE_URL; // Chỉ định đường dẫn 
  if(Firebase.signUp(&config, &auth, "", "")){ //Hai đối số ở cuối trống là người ẩn danh đăng kí 
    Serial.println("signUp OK");
    signupOK = true;
  }else{
    Serial.println(config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //Gán chức năng gọi lại tác vụ tạo mã thông báo
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){ //millis() : trả về thời gian kể từ khi ESP32 được khởi động, tính bằng s
  sendDataPrevMillis = millis();
    POTData = analogRead(POT);                                                                          
    voltage = (float)analogReadMilliVolts(POT)/1000;
    int bamxung = map(POTData, 0, 4095, 0, 255);
    analogWrite(LED, bamxung);
    String ledStatus = (bamxung > 0) ? "ON" : "OFF";
    Firebase.RTDB.setInt(&fbdo, "POT/Bam_Xung", bamxung);
    Firebase.RTDB.setFloat(&fbdo, "POT/vol", voltage);
    Firebase.RTDB.setString(&fbdo, "Led/Status", ledStatus);
  }
}

