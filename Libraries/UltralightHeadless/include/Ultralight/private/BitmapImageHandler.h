#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/Bitmap.h>

namespace ultralight {
	
class UExport BitmapImageHandler {
 public:
  virtual ~BitmapImageHandler();

  virtual uint64_t GetNextImageId() = 0;

  virtual void OnStorePlatformImageFrame(uint64_t image_id, uint32_t frame_id,
    uint32_t frame_count, Ref<Bitmap> bitmap, bool isDataComplete) = 0;

  virtual void OnDestroyPlatformImageFrame(uint64_t image_id, uint32_t frame_id) = 0;

  virtual void OnDestroyAllPlatformImageFramesExcept(uint64_t image_id,
    uint32_t frame_id) = 0;

  virtual void OnDestroyPlatformImage(uint64_t image_id) = 0;
};

}  // namespace ultralight
