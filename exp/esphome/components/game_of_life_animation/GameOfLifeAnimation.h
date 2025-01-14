#pragma once
#include "esphome/components/image/image.h"

namespace esphome
{
  namespace game_of_life_animation
  {

    class GameOfLifeAnimation : public image::Image
    {
    public:
      GameOfLifeAnimation(
        const uint8_t* data_start, 
        int width, 
        int height, 
        uint32_t animation_frame_count, 
        image::ImageType type);

      uint32_t get_frame_count() const { return this->animation_frame_count_; };
      uint32_t current_generation() const { return this->current_generation_; }; 
      void next_generation();

      /** Resets the animation to the given starting frame..
       *
       * @param frame The starting frame number of the GIF image the component was loaded at initalization.
       */
      void set_frame(uint32_t frame);

    private:
      const uint32_t image_size;
      const uint8_t* animation_data_start_;
      uint32_t animation_frame_count_;
      uint8_t* frame_buf0;
      uint8_t* frame_buf1;
      uint8_t* current_frame_buf;
      uint8_t* next_frame_buf;
      uint32_t current_generation_;

      uint32_t bitOffsetOfPixel(int x, int y) const;
      bool get_binary_pixel(uint8_t* buf, int x, int y) const;
      void set_binary_pixel(uint8_t* buf, int x, int y, bool state) const;
      int count_neighbors(uint8_t* buf, int x, int y);
    };

  } // namespace animation
} // namespace esphome
