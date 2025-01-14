#include "GameOfLifeAnimation.h"
#include "esphome/core/hal.h"

 #define LOG(...) esph_log_d("GoL", __VA_ARGS__)
//#define LOG(...)

namespace esphome
{
  namespace game_of_life_animation
  {
    GameOfLifeAnimation::GameOfLifeAnimation(
        const uint8_t *data_start,
        int width,
        int height,
        uint32_t animation_frame_count,
        image::ImageType type)
        : Image(data_start, width, height, type),
          image_size(image_type_to_width_stride(width, type) * height),
          animation_data_start_(data_start),
          animation_frame_count_(animation_frame_count),
          current_generation_(0)
    {
      frame_buf0 = (uint8_t *)malloc(image_size);
      std::memset(frame_buf0, 0, image_size);
      frame_buf1 = (uint8_t *)malloc(image_size);
      std::memset(frame_buf1, 0, image_size);

      current_frame_buf = frame_buf0;
      next_frame_buf = frame_buf1;

      set_frame(0);

      // hereafter, data_start_ points to the bitmap to be displayed.  We save animation_data_start_ as the frames of initial worlds
      // for the user to select with set_frame().  Upon construction, the initial frame is (a copy of) the first one.
    }

    void GameOfLifeAnimation::set_frame(uint32_t frame)
    {
      if (frame < this->animation_frame_count_)
      {
        uint n = image_size;
        uint8_t *dst = current_frame_buf;
        const uint8_t *src = this->animation_data_start_ + image_size * frame;

        LOG("w %3d h %3d", width_, height_);
        // LOG("fb0 0x%X", frame_buf0);
        // LOG("fb1 0x%X", frame_buf1);
        // LOG("cfb 0x%X", current_frame_buf);
        // LOG("dst 0x%X", dst);
        // LOG("src 0x%X", src);
        // LOG("size %d", n);

        while (n > 0)
        {
          uint8_t b = progmem_read_byte(src);
          // LOG("cpy: n %4d b %X", n, b);

          *dst = b;
          ++dst;
          ++src;
          --n;
        }

        // current_frame_buf = (uint8_t*) this->animation_data_start_ + image_size * frame;
        // LOG("cfb 0x%X", current_frame_buf);
        this->data_start_ = current_frame_buf;
        this->current_generation_ = 0;
      }
    }

    // count the number of live neighbors in the 8 adjacent cells
    int GameOfLifeAnimation::count_neighbors(uint8_t *buf, int x, int y)
    {
      int total =
          get_binary_pixel(buf, (x + 1) % this->width_, y) +
          get_binary_pixel(buf, x, (y + 1) % this->height_) +
          get_binary_pixel(buf, (x + this->width_ - 1) % this->width_, y) +
          get_binary_pixel(buf, x, (y + this->height_ - 1) % this->height_) +
          get_binary_pixel(buf, (x + 1) % this->width_, (y + 1) % this->height_) +
          get_binary_pixel(buf, (x + this->width_ - 1) % this->width_, (y + this->height_ - 1) % this->height_) +
          get_binary_pixel(buf, (x + 1) % this->width_, (y + this->height_ - 1) % this->height_) +
          get_binary_pixel(buf, (x + this->width_ - 1) % this->width_, (y + 1) % this->height_);
      return total;
    }

    void GameOfLifeAnimation::next_generation()
    {
      // looking at the world pointed to by current_frame_buf, set up the next generation in
      // the world pointed to by next_frame_buf.

      for (int y = 0; y < this->height_; y++)
      {
        for (int x = 0; x < this->width_; x++)
        {
          bool isAlive = get_binary_pixel(current_frame_buf, x, y);
          // LOG("-- x %3d y %3d isAlive %s ", x, y, isAlive ? "Y" : "N");

          int total = count_neighbors(current_frame_buf, x, y);
          // LOG("neighbors %d", total);

          if (isAlive)
          {
            if (total < 2 || total > 3)
            {
              isAlive = false;
            }
          }
          else if (total == 3)
          {
            isAlive = true;
          }
          set_binary_pixel(next_frame_buf, x, y, isAlive);
        }
      }

      uint8_t *temp = current_frame_buf;
      current_frame_buf = next_frame_buf;
      next_frame_buf = temp;

      this->data_start_ = this->current_frame_buf;
      this->current_generation_++;
    }

    uint32_t GameOfLifeAnimation::bitOffsetOfPixel(int x, int y) const
    {
      const uint32_t width_8 = 128; // ((this->width_ + 7u) / 8u) * 8u;
      // LOG("width_8 %u", width_8);  // nearest multiple of 8 bits.

      uint32_t pos = x + y * width_8;
      // LOG("x %u y %u pos %u", x, y, pos);

      return pos;
    }

    bool GameOfLifeAnimation::get_binary_pixel(uint8_t *buf, int x, int y) const
    {
      uint32_t pos = bitOffsetOfPixel(x, y);
      uint32_t byteOffset = (pos / 8u);
      uint8_t b = progmem_read_byte(buf + byteOffset);
      uint8_t bitInByte = pos % 8u;
      bool retval = b & (1 << bitInByte);
      // LOG("get: x %3d y %3d bo: %3d by: %2X bp %s", x, y, byteOffset, b, retval ? "1" : "0");
      return retval;
    }

    void GameOfLifeAnimation::set_binary_pixel(uint8_t *buf, int x, int y, bool state) const
    {
      uint32_t pos = bitOffsetOfPixel(x, y);
      uint32_t byteOffset = (pos / 8u);
      uint8_t b = progmem_read_byte(buf + byteOffset); // ?? needed?
      uint8_t bitInByte = pos % 8u;
      uint8_t bitMask = (1 << bitInByte);
      uint8_t a;
      if (state)
      {
        a = b | bitMask;
      }
      else
      {
        a = b & ~bitMask;
      }
      *(buf + byteOffset) = a;
      // LOG("set: x %3d y %3d pos: %4d bo: %3d by: %2X bit %d bm: %2X ba: %2X s %s", x, y, pos, byteOffset, b, bitInByte, bitMask, a, state ? "1" : "0");
    }

  } // namespace game_of_life_animation
} // namespace esphome
