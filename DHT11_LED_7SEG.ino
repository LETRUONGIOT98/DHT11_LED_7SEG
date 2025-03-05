#include "DHT.h"
#include <IRremote.h>

#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int strobePin = 4;  // Pin kết nối với Data (pin 2) của 4094 cho bộ 1
int dataPin = 5;    // Pin kết nối với Clock (pin 3) của 4094 cho bộ 1
int clockPin = 6;   // Pin kết nối với Strobe (pin 1) của 4094 cho bộ 1

int strobePin1 = 7; // Pin kết nối với Data (pin 2) của 4094 cho bộ 2
int dataPin1 = 8;   // Pin kết nối với Clock (pin 3) của 4094 cho bộ 2
int clockPin1 = 9;  // Pin kết nối với Strobe (pin 1) của 4094 cho bộ 2

// Mã 7 đoạn cho các ký tự số từ 0 đến 9
byte segChar[] = {0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0, 0xfe, 0xf6};
byte segChardot[] = {0xfd, 0x61, 0xdb, 0xf3, 0x67, 0xb7, 0xbf, 0xe1, 0xff, 0xf7};

const int RECV_PIN = A3; // Chân nhận tín hiệu IR trên Arduino Nano

IRrecv irrecv(RECV_PIN);

decode_results results;
float chinht = 0.0;
float chinhh = 0.0;
#define nut1 10
#define nut2 11
#define nut3 12
#define nut4 13
void setup() {
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(strobePin1, OUTPUT);
  pinMode(clockPin1, OUTPUT);
  pinMode(dataPin1, OUTPUT);

  pinMode(nut1, INPUT);
  pinMode(nut2, INPUT);
  pinMode(nut3, INPUT);
  pinMode(nut4, INPUT);
  Serial.begin(9600);
  dht.begin();
  irrecv.enableIRIn(); // Bắt đầu nhận mã IR
}

void loop() {
  

  if (irrecv.decode(&results)) {
    // In ra mã IR
    char hexString[9]; // Đủ để lưu 8 ký tự hexadecimals và ký tự kết thúc chuỗi '\0'
    sprintf(hexString, "%08X", results.value);
    Serial.print("Đã nhận mã IR: 0x");
    Serial.println(hexString);
    irrecv.resume(); // Tiếp tục nhận mã IR tiếp theo

    // Kiểm tra nếu mã IR nhận được là 1838
    if (strcmp(hexString, "000018E7") == 0) {
      chinht = chinht +0.1;
    }
    if (strcmp(hexString, "00004AB5") == 0) {
      chinht = chinht -0.1;
    }
    if (strcmp(hexString, "000010EF") == 0) {
      chinhh = chinhh -0.1;
    }
    if (strcmp(hexString, "00005AA5") == 0) {
      chinhh = chinhh + 0.1;
    }
  }
  if (digitalRead(nut1) == 0) {
      chinht = chinht + 0.1;
      delay(300);
    }
    if (digitalRead(nut2) == 0) {
      chinht = chinht - 0.1;
      delay(300);
    }
    if (digitalRead(nut3) == 0) {
      chinhh = chinhh - 0.1;
      delay(300);
    }
    if (digitalRead(nut4) == 0) {
      chinhh = chinhh + 0.1;
      delay(300);
    }
  float h = dht.readHumidity() + chinhh;
  float t = dht.readTemperature() + chinht;
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println();

  hienthit1(t);  // Hiển thị nhiệt độ trên bộ LED 1
  hienthit2(h);  // Hiển thị độ ẩm trên bộ LED 2
 
}

void hienthit1(float t) {
  unsigned char hundred = 0, ten = 0, unit = 0, decimal = 0;
  
  int intPart = int(t);                // Lấy phần nguyên của số
  decimal = int((t - intPart) * 10);   // Lấy phần thập phân đầu tiên

  hundred = char(intPart / 100);
  ten = char((intPart - hundred * 100) / 10);
  unit = char(intPart % 10);

  digitalWrite(strobePin, LOW);

  // Hiển thị phần thập phân, phần đơn vị, và phần chục theo thứ tự đúng
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[decimal]);       // Phần thập phân
  shiftOut(dataPin, clockPin, MSBFIRST, segChardot[unit]);       // Phần đơn vị có dấu chấm
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[ten]);           // Phần chục

  // Hiển thị hàng trăm nếu cần thiết (nếu nhiệt độ >= 100)
  if (hundred != 0) {
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[hundred]); 
  }

  digitalWrite(strobePin, HIGH);
  delay(10);
}

void hienthit2(float h) {
  unsigned char hundred1 = 0, ten1 = 0, unit1 = 0, decimal1 = 0;
  
  int intPart1 = int(h);               // Lấy phần nguyên của số
  decimal1 = int((h - intPart1) * 10); // Lấy phần thập phân đầu tiên

  hundred1 = char(intPart1 / 100);
  ten1 = char((intPart1 - hundred1 * 100) / 10);
  unit1 = char(intPart1 % 10);

  digitalWrite(strobePin1, LOW);

  // Hiển thị phần thập phân, phần đơn vị, và phần chục theo thứ tự đúng
  shiftOut(dataPin1, clockPin1, MSBFIRST, segChar[decimal1]);     // Phần thập phân
  shiftOut(dataPin1, clockPin1, MSBFIRST, segChardot[unit1]);     // Phần đơn vị có dấu chấm
  shiftOut(dataPin1, clockPin1, MSBFIRST, segChar[ten1]);         // Phần chục

  // Hiển thị hàng trăm nếu cần thiết (nếu độ ẩm >= 100)
  if (hundred1 != 0) {
    shiftOut(dataPin1, clockPin1, MSBFIRST, segChar[hundred1]); 
  }

  digitalWrite(strobePin1, HIGH);
  delay(10);
}
