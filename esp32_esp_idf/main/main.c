#include <stdlib.h>

#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_mqtt.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <a4988.h>
#include <ads111x.h>
#define WIFI_SSID "tracenet"
#define WIFI_PASS "fulanito_tr4c3"

#define MQTT_HOST "192.168.43.126"
#define MQTT_USER ""
#define MQTT_PASS ""
#define MQTT_PORT "1883"
#define SDA_GPIO 22
#define SCL_GPIO 21
#define ADDRESS ADS111X_ADDR_GND // connect ADDR pin to GND
#define GAIN ADS111X_GAIN_4V096


void ads111x_read(void *pvParamters){

    i2c_dev_t dev;
    while (i2cdev_init() != ESP_OK){
        printf("Could not init I2Cdev library\n");
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    while (ads111x_init_desc(&dev, ADDRESS, 0, SDA_GPIO, SCL_GPIO) != ESP_OK){
        printf("Could not init device descriptor\n");
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    ads111x_set_mode(&dev, ADS111X_MODE_CONTUNOUS);
    ads111x_set_data_rate(&dev, ADS111X_DATA_RATE_860);
    ads111x_set_input_mux(&dev, ADS111X_MUX_0_GND);
    ads111x_set_gain(&dev, GAIN);


    float gain_val = ads111x_gain_values[GAIN];
    for (;;){
        // wait for conversion end
        bool busy;
        do{
            ads111x_is_busy(&dev, &busy);
        }
        while (busy);

        // Read result
        int16_t raw = 0;
        if (ads111x_get_value(&dev, &raw) == ESP_OK){
            float voltage = gain_val / ADS111X_MAX_VALUE * raw;
            //printf("Raw ADC value: %d, voltage: %.04f volts\n", raw, voltage);
	    char buf[6];
	    sprintf(buf, "%.04f", voltage);
	    esp_mqtt_publish("/hello", (uint8_t *)buf, 5, 2, false);

        }
        else
            printf("Cannot read ADC value\n");
        //vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}



static void restart(void *_) {
  for (;;) {
    
    // stop and start mqtt every minute
    vTaskDelay(60000 / portTICK_PERIOD_MS);
    esp_mqtt_stop();
    esp_mqtt_start(MQTT_HOST, MQTT_PORT, "esp-mqtt", MQTT_USER, MQTT_PASS);
  }
}

static esp_err_t event_handler(void *ctx, system_event_t *event) {
  switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
      // connect to ap
      esp_wifi_connect();
      break;

    case SYSTEM_EVENT_STA_GOT_IP:
      // start mqtt
      esp_mqtt_start(MQTT_HOST, MQTT_PORT, "esp-mqtt", MQTT_USER, MQTT_PASS);
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      // stop mqtt
      esp_mqtt_stop();

      // reconnect wifi
      esp_wifi_connect();
      break;

    default:
      break;
  }

  return ESP_OK;
}

static void status_callback(esp_mqtt_status_t status) {
  switch (status) {
    case ESP_MQTT_STATUS_CONNECTED:
      // subscribe
      //esp_mqtt_subscribe("/hello", 2);
      break;
    case ESP_MQTT_STATUS_DISCONNECTED:
      // reconnect
      esp_mqtt_start(MQTT_HOST, MQTT_PORT, "esp-mqtt", MQTT_USER, MQTT_PASS);
      break;
  }
}

static void message_callback(const char *topic, uint8_t *payload, size_t len) {
  ESP_LOGI("test", "incoming: %s => %s (%d)", topic, payload, (int)len);
}

void app_main() {
  // initialize nvs flash
  ESP_ERROR_CHECK(nvs_flash_init());

  // initialize tcp/ip adapter
  tcpip_adapter_init();

  // register event handler
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

  // initialize wifi
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // set wifi storage to ram
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

  // set wifi mode
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  // prepare wifi config
  wifi_config_t wifi_config = {.sta = {.ssid = WIFI_SSID, .password = WIFI_PASS}};
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

  // start wifi
  ESP_ERROR_CHECK(esp_wifi_start());

  // initialize mqtt
  esp_mqtt_init(status_callback, message_callback, 256, 2000);

  // create tasks
  //xTaskCreatePinnedToCore(restart, "restart", 2048, NULL, 10, NULL, 1);
  //
  //ads1115_t ads = ads1115_config(I2C_MASTER_NUM, ADS1115_ADDR);
  // CA4988Stepper stepper(2, 15, 800);
  int steps = 800*3;
  int rpm = 60*5;
  stepper_init(21, 18, 800, LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, LEDC_CHANNEL_0, PCNT_UNIT_0, PCNT_CHANNEL_0);
  set_speed_rpm(rpm,false);
  step(steps,portMAX_DELAY, false);
  run(-1, false);
  //stop(true);
  
  //xTaskCreatePinnedToCore(ads111x_read, "ads111x", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
  //xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);

}
