#include <SPI.h>
#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2     // データ(黄)で使用するポート番号
#define ONE_WIRE_BUS_2 3   // 新しい温度センサのデータ(黄)で使用するポート番号
#define SENSER_BIT 10      // 精度の設定bit

//イーサネットシールドでは、CSはピン4です。CSピンとして使用されていなくても、ハードウェアCSピン（ほとんどのArduinoボードでは10、メガでは53）を出力のままにする必要があります。
const int chipSelect = 10;
OneWire oneWire(ONE_WIRE_BUS);
OneWire oneWire2(ONE_WIRE_BUS_2);  // 新しい温度センサのワイヤーライブラリの初期化
DallasTemperature sensors(&oneWire);
DallasTemperature sensors2(&oneWire2);  // 新しい温度センサのDallasTemperatureライブラリの初期化



void setup() {
   // シリアル通信を開き、ポートが開くのを待ちます
   Serial.begin(9600);
   while (!Serial) {
       ; // シリアルポートが接続するのを待ちます。レオナルドのみに必要
   }

   Serial.print("Initializing SD card...");
   // デフォルトのチップセレクトピンが使用されていない場合でも、それが出力に設定されていることを確認してください。
   pinMode(10, OUTPUT);

   // 使用するchipSelectピンも出力に設定する必要があります。
   pinMode(chipSelect, OUTPUT);

   sensors.setResolution(SENSER_BIT);
   sensors2.setResolution(SENSER_BIT);  // 新しい温度センサの精度の設定
   // カードが存在し、初期化できるかどうか確認してください。
   if (!SD.begin(chipSelect)) {
       Serial.println("Card failed, or not present");
       // 何もしないでください
       return;
   }
   Serial.println("card initialized.");
}

void loop() {
   //delay(1000);  // 適宜調整
   // ログにデータを組み立てるための文字列を作成します。
   //String dataString = "";
   sensors.requestTemperatures();              // 既存の温度センサから温度取得要求
   Serial.print("Sensor 1: ");
   Serial.println(sensors.getTempCByIndex(0)); // 既存の温度センサの温度の取得&シリアル送信

   sensors2.requestTemperatures();             // 新しい温度センサから温度取得要求
   Serial.print("Sensor 2: ");
   Serial.println(sensors2.getTempCByIndex(0));// 新しい温度センサの温度の取得&シリアル送信
   delay(1000);
  


   // ファイルを開きます。一度に開くことができるファイルは1つだけなので、別のファイルを開く前にこのファイルを閉じる必要があります。
   File dataFile = SD.open("datalog.csv", FILE_WRITE);

   // ファイルが利用可能な場合は、そのファイルに書き込みます。
   if (dataFile) {
       dataFile.println(sensors.getTempCByIndex(0));
       dataFile.println(sensors2.getTempCByIndex(0));
       dataFile.close();
       // シリアルポートにも出力します
       //Serial.println(dataString);
   }

   // ファイルが開いていない場合は、エラーをポップアップします。
   else {
       Serial.println("error opening datalog.txt");
   }
}