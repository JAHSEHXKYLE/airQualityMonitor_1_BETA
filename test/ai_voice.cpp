#include <WiFi.h>
#include <Arduino.h>
#include "ai_vox_engine.h"
#include "ai_vox_observer.h"
#include "i2s_std_audio_input_device.h"
#include "i2s_std_audio_output_device.h"

#ifndef WIFI_SSID
#define WIFI_SSID "JAHSEHKYLE LINK"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "1234567890"
#endif

namespace {

constexpr gpio_num_t kMicPinBclk = GPIO_NUM_5;
constexpr gpio_num_t kMicPinWs = GPIO_NUM_4;
constexpr gpio_num_t kMicPinDin = GPIO_NUM_6;

constexpr gpio_num_t kSpeakerPinBclk = GPIO_NUM_15;
constexpr gpio_num_t kSpeakerPinWs = GPIO_NUM_16;
constexpr gpio_num_t kSpeakerPinDout = GPIO_NUM_7;

constexpr gpio_num_t kTriggerPin = GPIO_NUM_8;

auto g_observer = std::make_shared<ai_vox::Observer>();
std::shared_ptr<ai_vox::iot::Entity> g_speaker_iot_entity;
std::shared_ptr<ai_vox::iot::Entity> indoor_air_quality_iot_entity;
auto g_audio_output_device = std::make_shared<ai_vox::I2sStdAudioOutputDevice>(kSpeakerPinBclk, kSpeakerPinWs, kSpeakerPinDout);

void InitIot() {
    printf("InitIot\n");
    auto& ai_vox_engine = ai_vox::Engine::GetInstance();

    std::vector<ai_vox::iot::Property> indoor_air_quality_properties({
        {
            "pm1.0",
            "室内PM1.0浓度",
            ai_vox::iot::ValueType::kNumber
        },
        {
            "pm2.5",
            "室内PM2.5浓度",
            ai_vox::iot::ValueType::kNumber
        },
        {
            "pm10.0",
            "室内PM10.0浓度",
            ai_vox::iot::ValueType::kNumber
        },
        {
            "TVOC",
            "室内TVOC浓度 单位ppb",
            ai_vox::iot::ValueType::kNumber
        },
        {
            "CO2",
            "室内二氧化碳浓度 单位ppm",
            ai_vox::iot::ValueType::kNumber
        },
        {
            "CH2O",
            "室内甲醛浓度 单位ppm",
            ai_vox::iot::ValueType::kString
        },
        {
            "humidity",
            "室内湿度 单位%RH",
            ai_vox::iot::ValueType::kString
        },
        {
            "temperature",
            "室内温度",
            ai_vox::iot::ValueType::kString
        },
        {
            "pressure",
            "室内气压",
            ai_vox::iot::ValueType::kString
        },
    });

    std::vector<ai_vox::iot::Function> indoor_air_quality_functions({
        {"GetIndoorAirQuality",  // function name
            "获取当前室内空气质量",  // function description
            {
           
            }
        },
    });

    indoor_air_quality_iot_entity = std::make_shared<ai_vox::iot::Entity>(
        "IndoorAirQuality",  // name
        "室内空气质量检测器",  // description
        std::move(indoor_air_quality_properties),  // properties
        std::move(indoor_air_quality_functions)  // functions
    );
    indoor_air_quality_iot_entity->UpdateState("pm1.0", 25);
    indoor_air_quality_iot_entity->UpdateState("pm2.5", 100);
    indoor_air_quality_iot_entity->UpdateState("pm10.0", 100);
    indoor_air_quality_iot_entity->UpdateState("TVOC", 1000);
    indoor_air_quality_iot_entity->UpdateState("CO2", 1000);
    indoor_air_quality_iot_entity->UpdateState("CH2O", std::to_string(0.001f));
    indoor_air_quality_iot_entity->UpdateState("humidity", std::to_string(50.0f));
    indoor_air_quality_iot_entity->UpdateState("temperature", std::to_string(25.0f));
    indoor_air_quality_iot_entity->UpdateState("pressure", std::to_string(1013.25f));
    // Register the indoor air quality entity with the AI Vox engine
    ai_vox_engine.RegisterIotEntity(indoor_air_quality_iot_entity);

  // Register the observer with the AI Vox engine
  // Speaker
  // 1.Define the properties for the speaker entity
    std::vector<ai_vox::iot::Property> speaker_properties({
        {
            "volume",                        // property name
            "当前音量值",                    // property description
            ai_vox::iot::ValueType::kNumber  // property type: number, string or bool
        },
        // add more properties as needed
    });

  // 2.Define the functions for the speaker entity
    std::vector<ai_vox::iot::Function> speaker_functions({
        {"SetVolume",  // function name
        "设置音量",   // function description
        {
            {
                "volume",                         // parameter name
                "0到100之间的整数",               // parameter description
                ai_vox::iot::ValueType::kNumber,  // parameter type
                true                              // parameter required
            },
            // add more parameters as needed
        }},
        // add more functions as needed
    });

  // 3.Create the speaker entity
    g_speaker_iot_entity = std::make_shared<ai_vox::iot::Entity>(
        "Speaker",                      // name
        "扬声器",                       // description
        std::move(speaker_properties),  // properties
        std::move(speaker_functions)    // functions
    );

  // 4.Initialize the speaker entity with default values
    g_speaker_iot_entity->UpdateState("volume", g_audio_output_device->volume());

  // 5.Register the speaker entity with the AI Vox engine
    ai_vox_engine.RegisterIotEntity(g_speaker_iot_entity);

    }
   
}  // namespace


void setup() {
  Serial.begin(115200);
  printf("Init\n");

//   if (heap_caps_get_total_size(MALLOC_CAP_SPIRAM) > 0) {
//     WiFi.useStaticBuffers(true);
//   } else {
//     WiFi.useStaticBuffers(false);
//   }

  printf("Connecting to WiFi, ssid: %s, password: %s\n", WIFI_SSID, WIFI_PASSWORD);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    printf("Connecting to WiFi, ssid: %s, password: %s\n", WIFI_SSID, WIFI_PASSWORD);
  }

  printf("WiFi connected, IP address: %s\n", WiFi.localIP().toString().c_str());

//   pinMode(kLedPin, OUTPUT);
//   digitalWrite(kLedPin, LOW);
  
}

void loop() {
    InitIot();

    auto audio_input_device = std::make_shared<ai_vox::I2sStdAudioInputDevice>(kMicPinBclk, kMicPinWs, kMicPinDin);

    auto& ai_vox_engine = ai_vox::Engine::GetInstance();
    ai_vox_engine.SetObserver(g_observer);
    ai_vox_engine.SetTrigger(kTriggerPin);
    ai_vox_engine.SetOtaUrl("https://api.tenclass.net/xiaozhi/ota/");
    ai_vox_engine.ConfigWebsocket("wss://api.tenclass.net/xiaozhi/v1/",
                                    {
                                        {"Authorization", "Bearer test-token"},
                                    });
    ai_vox_engine.Start(audio_input_device, g_audio_output_device);
    printf("AI Vox engine started\n");
    while (1)
    {
        const auto events = g_observer->PopEvents();
        for (auto& event : events) {
            if (auto activation_event = std::get_if<ai_vox::Observer::ActivationEvent>(&event)) {
                printf("activation code: %s, message: %s\n", activation_event->code.c_str(), activation_event->message.c_str());
            } else if (auto state_changed_event = std::get_if<ai_vox::Observer::StateChangedEvent>(&event)) {
                printf("state changed from %" PRIu8 " to %" PRIu8 "\n",
                static_cast<uint8_t>(state_changed_event->old_state),
                static_cast<uint8_t>(state_changed_event->new_state));
                switch (state_changed_event->new_state) {
                    case ai_vox::ChatState::kIdle: {
                        printf("Idle\n");
                        break;
                    }
                    case ai_vox::ChatState::kIniting: {
                        printf("Initing...\n");
                        break;
                    }
                    case ai_vox::ChatState::kStandby: {
                        printf("Standby\n");
                        break;
                    }
                    case ai_vox::ChatState::kConnecting: {
                        printf("Connecting...\n");
                        break;
                    }
                    case ai_vox::ChatState::kListening: {
                        printf("Listening...\n");
                        break;
                    }
                    case ai_vox::ChatState::kSpeaking: {
                        printf("Speaking...\n");
                        break;
                    }
                        default: {
                        break;
                    }
                }
            } else if (auto emotion_event = std::get_if<ai_vox::Observer::EmotionEvent>(&event)) {
                printf("emotion: %s\n", emotion_event->emotion.c_str());
            } else if (auto chat_message_event = std::get_if<ai_vox::Observer::ChatMessageEvent>(&event)) {
                switch (chat_message_event->role) {
                    case ai_vox::ChatRole::kAssistant: {
                        printf("role: assistant, content: %s\n", chat_message_event->content.c_str());
                        break;
                    }
                    case ai_vox::ChatRole::kUser: {
                        printf("role: user, content: %s\n", chat_message_event->content.c_str());
                        break;
                    }
                }
            } else if (auto iot_message_event = std::get_if<ai_vox::Observer::IotMessageEvent>(&event)) {
                printf("IOT message: %s, function: %s\n", iot_message_event->name.c_str(), iot_message_event->function.c_str());
                for (const auto& [key, value] : iot_message_event->parameters) {
                    if (std::get_if<bool>(&value)) {
                        printf("key: %s, value: %s\n", key.c_str(), std::get<bool>(value) ? "true" : "false");
                    } else if (std::get_if<std::string>(&value)) {
                        printf("key: %s, value: %s\n", key.c_str(), std::get<std::string>(value).c_str());
                    } else if (std::get_if<int64_t>(&value)) {
                        printf("key: %s, value: %lld\n", key.c_str(), std::get<int64_t>(value));
                    }
                }
                if (iot_message_event->name == "IndoorAirQuality") {
                    if (iot_message_event->function == "GetIndoorAirQuality") {
                        printf("GetIndoorAirQuality\n");
                        indoor_air_quality_iot_entity->UpdateState("pm2.5", random(0, 100));
                        indoor_air_quality_iot_entity->UpdateState("pm10.0", random(0, 100));
                    }
                }
                else if (iot_message_event->name == "Speaker") {
                    if (iot_message_event->function == "SetVolume") {
                        if (const auto it = iot_message_event->parameters.find("volume"); it != iot_message_event->parameters.end()) {
                            auto volume = it->second;
                            if (std::get_if<int64_t>(&volume)) {
                                printf("Speaker volume: %lld\n", std::get<int64_t>(volume));
                                g_audio_output_device->SetVolume(std::get<int64_t>(volume));
                                g_speaker_iot_entity->UpdateState("volume", std::get<int64_t>(volume));  // Note: Must UpdateState after change the device state
                            }
                        } 
                    }
                }
            }
        }
        taskYIELD();
    }
}