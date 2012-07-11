#include "ColorReference.h"

libmspub::Color libmspub::ColorReference::getRealColor(unsigned c, const std::vector<Color> &palette) const
{
  unsigned char type = (c >> 24) & 0xFF;
  if (type == 0x08)
  {
    if ((c & 0xFFFFFF) >= palette.size())
    {
      return Color();
    }
    return palette[c & 0xFFFFFF];
  }
  return Color(c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF);
}
libmspub::Color libmspub::ColorReference::getFinalColor(const std::vector<Color> &palette) const
{
  unsigned char modifiedType = (m_modifiedColor >> 24) & 0xFF;
  if (modifiedType == CHANGE_INTENSITY)
  {
    Color c = getRealColor(m_baseColor, palette);
    unsigned char changeIntensityBase = (m_modifiedColor >> 8) & 0xFF;
    double intensity = (double)((m_modifiedColor >> 16) & 0xFF) / 0xFF;
    if (changeIntensityBase == BLACK_BASE)
    {
      return Color(c.r * intensity, c.g * intensity, c.b * intensity);
    }
    if (changeIntensityBase == WHITE_BASE)
    {
      return Color(c.r + (255 - c.r) * (1 - intensity), c.g + (255 - c.g) * (1 - intensity), c.b + (255 - c.b) * (1 - intensity));
    }
    return Color();
  }
  else
  {
    return getRealColor(m_modifiedColor, palette);
  }
}

namespace libmspub
{
bool operator==(const libmspub::ColorReference &l, const libmspub::ColorReference &r)
{
  return l.m_baseColor == r.m_baseColor && l.m_modifiedColor == r.m_modifiedColor;
}

// const unsigned char ColorReference::COLOR_PALETTE;

const unsigned char ColorReference::CHANGE_INTENSITY;

const unsigned char ColorReference::BLACK_BASE;

const unsigned char ColorReference::WHITE_BASE;

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
