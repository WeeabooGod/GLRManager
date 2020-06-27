#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>

namespace ultralight {

class Bitmap;

class UExport Image : public RefCounted {
public:
  static Ref<Image> Create();

  virtual void SetFrame(uint32_t frame_id, uint32_t frame_count, Ref<Bitmap> bitmap, bool is_data_complete) = 0;

  virtual uint32_t frame_count() const = 0;

  virtual bool GetFrameSize(uint32_t frame_id, uint32_t& width, uint32_t& height) = 0;

  virtual void ClearFrame(uint32_t frame_id) = 0;

protected:
  Image();
  virtual ~Image();
  Image(const Image&);
  void operator=(const Image&);
};

}  // namespace ultralight
