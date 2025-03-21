#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <driver/i2s.h>
#include "esp_speech_recognition.h"  // ESP-SR头文件（需根据实际框架调整）

// WS2812灯珠配置
#define LED_PIN 48          // WS2812连接到GPIO48
#define LED_COUNT 1         // 灯珠数量
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// I2S配置（INMP441麦克风）
#define I2S_WS 4            // WS引脚连接到GPIO4
#define I2S_SCK 5           // SCK引脚连接到GPIO5
#define I2S_SD 6            // SD引脚连接到GPIO6
#define I2S_SAMPLE_RATE 16000  // 采样率16kHz
#define I2S_BUFFER_SIZE 1024   // 音频缓冲区大小

// ESP-SR配置
esp_sr_wakeword_handle_t wakeword_handle;  // 唤醒词检测句柄
const char* wakeword = "你好乐鑫";         // 定义唤醒词

void setup() {
  Serial.begin(115200);     // 初始化串口通信
  strip.begin();            // 初始化WS2812灯珠
  strip.show();             // 确保灯珠初始状态为关闭

  // 配置I2S接口
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),  // 主模式，接收音频
    .sample_rate = I2S_SAMPLE_RATE,                       // 采样率
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,         // 16位采样
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,          // 单声道（左声道）
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,    // 标准I2S格式
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,             // 中断级别
    .dma_buf_count = 8,                                   // DMA缓冲区数量
    .dma_buf_len = I2S_BUFFER_SIZE,                       // 每个缓冲区大小
    .use_apll = false,                                    // 不使用APLL
    .tx_desc_auto_clear = false,                          // 不自动清除TX描述符
    .fixed_mclk = 0                                       // 默认MCLK
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,      // SCK引脚
    .ws_io_num = I2S_WS,        // WS引脚
    .data_out_num = I2S_PIN_NO_CHANGE,  // 无数据输出
    .data_in_num = I2S_SD       // SD引脚（音频输入）
  };

  // 安装I2S驱动并设置引脚
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);

  // 初始化ESP-SR唤醒词检测
  wakeword_handle = esp_sr_wakeword_init();              // 初始化ESP-SR
  esp_sr_wakeword_set_wakeword(wakeword_handle, wakeword);  // 设置唤醒词
}

void loop() {
  // 读取I2S音频数据
  int16_t samples[I2S_BUFFER_SIZE];
  size_t bytes_read = 0;
  i2s_read(I2S_NUM_0, samples, sizeof(samples), &bytes_read, portMAX_DELAY);

  // 使用ESP-SR检测唤醒词
  bool wakeWordDetected = esp_sr_wakeword_detect(wakeword_handle, samples, I2S_BUFFER_SIZE);

  // 根据检测结果设置灯珠颜色
  if (wakeWordDetected) {
    strip.setPixelColor(0, strip.Color(0, 255, 0));  // 检测到唤醒词，显示绿色
    Serial.println("检测到唤醒词：你好乐鑫");
  } else {
    strip.setPixelColor(0, strip.Color(255, 0, 0));  // 未检测到唤醒词，显示红色
  }
  strip.show();  // 更新灯珠状态

  delay(100);    // 控制循环频率，避免过快
}