#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/Geometry.h>

namespace ultralight {

class Bitmap;
class Path;
struct TextureInfo;

class UExport Font : public RefCounted {
public:
  virtual bool is_distance_field() const = 0;

  // Font size in pixels
  virtual float font_size() const = 0;

  // Scale factor that is used to convert pixels to layout units
  virtual double font_scale() const = 0;

  virtual bool HasGlyph(uint32_t index) const = 0;

  // Store a raster bitmap glyph (all 'doubles' should be pre-scaled to layout units using font_scale())
  virtual void StoreGlyph(uint32_t index, double advance, double width, double height, double bearing, RefPtr<Bitmap> bitmap, const Point& offset) = 0;

  // Store a SDF glyph (all 'doubles' should be pre-scaled to layout units using font_scale())
  virtual void StoreGlyph(uint32_t index, double advance, double width, double height, double bearing, RefPtr<Path> path) = 0;

  // Pre-scaled to layout units
  virtual double GetGlyphAdvance(uint32_t index) = 0;

  // Pre-scaled to layout units
  virtual double GetGlyphWidth(uint32_t index) = 0;

  // Pre-scaled to layout units
  virtual double GetGlyphHeight(uint32_t index) = 0;

  // Pre-scaled to layout units
  virtual double GetGlyphBearing(uint32_t index) = 0;

  // Get texture (creates one if doesn't exist)
  virtual TextureInfo GetGlyphTexture(uint32_t index) = 0;

  // Get bitmap (may be null if moved to a texture after call to GetGlyphTexture())
  virtual RefPtr<Bitmap> GetGlyphBitmap(uint32_t index) = 0;

  // Get glyph's bitmap offset (in pixels, may be non-integer if is distance field)
  virtual Point GetGlyphOffset(uint32_t index) = 0;

  virtual uint64_t bytes_used() = 0;

protected:
  static Ref<Font> Create(bool is_distance_field, float font_size, double font_scale);

  Font();
  virtual ~Font();
  Font(const Font&);
  void operator=(const Font&);

  friend class FontCache;
};

} // namespace ultralight
