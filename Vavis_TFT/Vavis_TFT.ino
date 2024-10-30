#include <lvgl.h>
#include <TFT_eSPI.h>
#include <LittleFS.h>
#include <FS.h>
#include <ArduinoJson.h>

#include "custom_font.c"
#include "over_load.c"
#include "pins.h"

extern const char *sevenSegmentJson;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 1];

int current_number = 0;
lv_obj_t *label;
lv_obj_t *arrow_up;
lv_obj_t *arrow_down;
lv_timer_t *timer;

void log_print(lv_log_level_t level, const char *buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

void screen_overload() {
  lv_obj_clean(lv_scr_act());
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  LV_FONT_DECLARE(over_load);
  label = lv_label_create(scr);
  lv_label_set_text_fmt(label, "OVERLOAD");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_font(label, &over_load, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xCC0000), 0);
}

void screen_evacuation() {
  lv_obj_clean(lv_scr_act());
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  LV_FONT_DECLARE(evacuation_mode);
  lv_obj_t *label1 = lv_label_create(scr);
  lv_label_set_text(label1, "EVACUATION");
  lv_obj_align(label1, LV_ALIGN_CENTER, 0, -10);
  lv_obj_set_style_text_font(label1, &evacuation_mode, 0);
  lv_obj_set_style_text_color(label1, lv_color_hex(0xCC0000), 0);

  lv_obj_t *label2 = lv_label_create(scr);
  lv_label_set_text(label2, "MODE");
  lv_obj_align(label2, LV_ALIGN_CENTER, 0, 50);
  lv_obj_set_style_text_font(label2, &evacuation_mode, 0);
  lv_obj_set_style_text_color(label2, lv_color_hex(0xCC0000), 0);
}

void draw_image(void) {
  lv_obj_t *background = lv_img_create(lv_scr_act());
  lv_img_set_src(background, "S:/background.bmp");
  lv_obj_align(background, LV_ALIGN_CENTER, 0, 0);

  arrow_up = lv_img_create(lv_scr_act());
  lv_img_set_src(arrow_up, "S:/arrow_up.bmp");
  lv_obj_align(arrow_up, LV_ALIGN_OUT_TOP_RIGHT, 30, 50);

  arrow_down = lv_img_create(lv_scr_act());
  lv_img_set_src(arrow_down, "S:/arrow_down.bmp");
  lv_obj_align(arrow_down, LV_ALIGN_OUT_TOP_RIGHT, 30, 50);

  lv_obj_add_flag(arrow_up, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(arrow_down, LV_OBJ_FLAG_HIDDEN);
}

void IRAM_ATTR zeroCrossingInterrupt() {
  bool up_event = digitalRead(UP) == LOW;
  bool down_event = digitalRead(DOWN) == LOW;

  if (up_event) {
    lv_obj_clear_flag(arrow_up, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arrow_down, LV_OBJ_FLAG_HIDDEN);
  } else if (down_event) {
    lv_obj_add_flag(arrow_up, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(arrow_down, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(arrow_up, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arrow_down, LV_OBJ_FLAG_HIDDEN);
  }

  if (digitalRead(INTRPT) == HIGH) {
    screen_overload();
  } else if (digitalRead(STP) == HIGH) {
    screen_evacuation();
  } else {
    // lv_obj_clean(lv_scr_act());
    // draw_image();
  }
}

void updateDisplay() {
  String binaryString = "";
  binaryString += digitalRead(SA) == LOW ? '1' : '0';
  binaryString += digitalRead(SB) == LOW ? '1' : '0';
  binaryString += digitalRead(SC) == LOW ? '1' : '0';
  binaryString += digitalRead(SD) == LOW ? '1' : '0';
  binaryString += digitalRead(SE) == LOW ? '1' : '0';
  binaryString += digitalRead(SF) == LOW ? '1' : '0';
  binaryString += digitalRead(SG) == LOW ? '1' : '0';

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, sevenSegmentJson);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  if (doc.containsKey(binaryString.c_str())) {
    current_number = doc[binaryString.c_str()].as<int>();
  } else {
    current_number = -1;
  }

  LV_FONT_DECLARE(custom_font);
  label = lv_label_create(lv_scr_act());
  lv_label_set_text_fmt(label, "%d", current_number);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 90);
  lv_obj_set_style_text_font(label, &custom_font, 0);
}

void setPinModes() {
  pinMode(SA, INPUT);
  pinMode(SB, INPUT);
  pinMode(SC, INPUT);
  pinMode(SD, INPUT);
  pinMode(SE, INPUT);
  pinMode(SF, INPUT);
  pinMode(SG, INPUT);

  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(UP), zeroCrossingInterrupt, ONHIGH);
  attachInterrupt(digitalPinToInterrupt(DOWN), zeroCrossingInterrupt, ONHIGH);

  pinMode(INTRPT, INPUT_PULLUP);
  pinMode(STP, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTRPT), zeroCrossingInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(STP), zeroCrossingInterrupt, CHANGE);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting LVGL");

  setPinModes();
  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(log_print);
#endif

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  Serial.println("LittleFS mounted successfully");

  lv_disp_t *disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_disp_set_rotation(disp, LV_DISPLAY_ROTATION_0);

  draw_image();
}

void loop() {
  lv_timer_handler();
  updateDisplay();
  // lv_tick_inc(1);  // Increment LVGL tick count by 1 ms
  // delay(1);
}
