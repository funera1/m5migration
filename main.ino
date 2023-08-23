#include <M5StickCPlus.h>
#include <esp_http_client.h>
#include <WiFi.h>
#include "time.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>
#include <MadgwickAHRS.h>

Madgwick MadgwickFilter;

float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;
float base_yaw = 0.0F;
float gx, gy, gz;
float ax, ay, az;

// 状態遷移: 0 -> 1 -> 2 -> 3 -> 0
// 0: 初期状態 1: BtnAを押す 2: 角速度が一定を上回った 3: 角速度が一定を下回った
int migration_switch_statement = 0;

typedef struct {
  char* host;
  int port;
} host_info_t;


const char* ssid = "";
const char* password = "";
host_info_t host_popos = {
  "",
  8000
};
host_info_t host_popos2 = {
  "",
  8001
};
host_info_t host_raspi = {
  "",
  8000
};

const int BUFFER_MAX_SIZE = 2048;
static const char *TAG = "HTTP_CLIENT";


// 0->どっちも走ってない, 1->Aが走ってる, 2->Bが走ってる
host_info_t *A, *B;
int running = 0;
int enable_migration = 1;

void setup() {
    M5.begin(); 
    Serial.begin(9600);

    // IMU 初期化
    M5.IMU.Init();
    MadgwickFilter.begin(100); //100Hz
    calibration();

    // Wifi接続
    M5.Lcd.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      M5.Lcd.print(".");
    }
    M5.Lcd.println(" CONNECTED");

    // host設定
    A = &host_popos;
    B = &host_popos2;
}

void loop() {
  M5.update();

  // 初期化
  if (M5.BtnB.wasPressed()) {
    stop_app(A);
    stop_app(B);
    init_app(A);
    init_app(B);
    start_app(A);
    running = 1;
    enable_migration = 1;
    migration_switch_statement = -1;
    M5.Lcd.fillScreen(BLACK);

    return;
  }

  calcAngle();
  Serial.printf("%7.2f\n", sqrt(gx*gx+gy*gy+gz*gz));

  // Log
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.printf(" stmt = %d\n", migration_switch_statement);
  M5.Lcd.printf(" pitch = %7.2f\n roll = %7.2f\n yaw = %7.2f\n", roll, pitch, yaw);
  M5.Lcd.printf(" base_yaw = %7.2f\n", base_yaw);

  // statement transition
  if (M5.BtnA.wasPressed()) {
    base_yaw = yaw;
    migration_switch_statement = 0;
  }

  float swing_speed = sqrt(gx*gx + gy*gy + gz*gz);
  if (migration_switch_statement == 0 && swing_speed > 800) {
    migration_switch_statement = 1;
  }
  
  if (migration_switch_statement == 1 && swing_speed < 50) {
    migration_switch_statement = 2;
  }

  if (migration_switch_statement == 2) {
    // A -> B
    if (yaw - base_yaw > 30 && running == 1 && enable_migration) {
      migration_switch_statement = -1;
      enable_migration = 0;
      migrate_flow(A, B);
      running = 2;
      enable_migration = 1;
      return;
    }
    // B -> A
    else if (yaw - base_yaw < -30 && running == 2 && enable_migration) {
      migration_switch_statement = -1;
      enable_migration = 0;
      migrate_flow(B, A);
      running = 1;
      enable_migration = 1;
      return;
    }
    else {
      migration_switch_statement = 0;
    }
  }
}

