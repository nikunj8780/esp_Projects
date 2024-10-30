#include <lvgl.h>
#include <TFT_eSPI.h>
#include <LittleFS.h>
#include <FS.h>
#include "custom_font.c"
#include "over_load.c"


// #define SA 1
// #define SB 2
// #define SC 4
// #define SD 5
// #define SE 6
// #define SF 7
// #define SG 15

// #define SA2 16
// #define SB2 17
// #define SC2 22
// #define SD2 23
// #define SE2 24
// #define SF2 38(Optional)
// #define SG2 39(Optional)

// #define UP 33
// #define DOWN 34

// #define INTRPT 35
// #define STP 36

// #define INPSLCT 37

// #define IDST1 47
// #define IDST2 48
// #define IDST3 38(Optional)
// #define IDST4 39(Optional)

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 1];

int current_number = 0;
lv_obj_t *label;
lv_obj_t *arrow_up;
lv_obj_t *arrow_down;
int direction = 1;
lv_timer_t *timer;

bool overload_screen_shown = false;

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

  LV_FONT_DECLARE(custom_font);
  label = lv_label_create(lv_scr_act());
  lv_label_set_text_fmt(label, "%d", current_number);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 90);
  lv_obj_set_style_text_font(label, &custom_font, 0);

  // show only the up arrow
  lv_obj_clear_flag(arrow_up, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(arrow_down, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t *evacuation_mode = lv_img_create(lv_scr_act());
  lv_img_set_src(evacuation_mode, "S:/evacuation_mode.bmp");
  lv_obj_align(evacuation_mode, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *over_load = lv_img_create(lv_scr_act());
  lv_img_set_src(over_load, "S:/over_load.bmp");
  lv_obj_align(over_load, LV_ALIGN_CENTER, 0, 0);
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting LVGL");

  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);

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

  // screen_overload();
  screen_evacuation();
  // draw_image();
  // timer = lv_timer_create(update_number, 3000, NULL);
}

void loop() {
  lv_timer_handler();
  lv_tick_inc(1);
  delay(1);
}






// void update_number(lv_timer_t *timer) {
//     lv_label_set_text_fmt(label, "%d", current_number);
//     Serial.print("Current number: ");
//     Serial.println(current_number);
//     current_number += direction;

//     if (direction == 1) {
//         lv_obj_clear_flag(arrow_up, LV_OBJ_FLAG_HIDDEN);
//         lv_obj_add_flag(arrow_down, LV_OBJ_FLAG_HIDDEN);
//     } else {
//         lv_obj_clear_flag(arrow_down, LV_OBJ_FLAG_HIDDEN);
//         lv_obj_add_flag(arrow_up, LV_OBJ_FLAG_HIDDEN);
//     }
//     if (current_number > 9) {
//         current_number = 9;
//         direction = -1;
//     } else if (current_number < 0) {
//         current_number = 0;
//         direction = 1;
//     }
// }

