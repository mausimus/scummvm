/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "ags/lib/allegro/gfx.h"
#include "ags/lib/allegro/flood.h"
#include "ags/ags.h"
#include "common/textconsole.h"
#include "graphics/screen.h"

namespace AGS3 {

int color_conversion;

// For Allegro, paletted sprites always use index 0 as the transparent color,
// and for higher resolution formats uses bright pink RGB 255, 0, 255
#define TRANSPARENT_COLOR(BITMAP) ((BITMAP).format.bytesPerPixel == 1 ? 0 : \
	(BITMAP).format.RGBToColor(255, 0, 255))

/*-------------------------------------------------------------------*/

BITMAP::BITMAP(Graphics::ManagedSurface *owner) : _owner(owner),
		w(owner->w), h(owner->h), format(owner->format),
		clip(false), ct(0), cl(0), cr(owner->w), cb(owner->h) {
	line.resize(h);
	for (uint y = 0; y < h; ++y)
		line[y] = (byte *)_owner->getBasePtr(0, y);
}

int BITMAP::getpixel(int x, int y) const {
	if (x < 0 || y < 0 || x >= w || y >= h)
		return -1;

	const byte *pixel = (const byte *)getBasePtr(x, y);
	if (format.bytesPerPixel == 1)
		return *pixel;
	else if (format.bytesPerPixel == 2)
		return *(const uint16 *)pixel;
	else
		return *(const uint32 *)pixel;
}

void BITMAP::circlefill(int x, int y, int radius, int color) {
	int cx = 0;
	int cy = radius;
	int df = 1 - radius;
	int d_e = 3;
	int d_se = -2 * radius + 5;

	do {
		_owner->hLine(x - cy, y - cx, x + cy, color);

		if (cx)
			_owner->hLine(x - cy, y + cx, x + cy, color);

		if (df < 0) {
			df += d_e;
			d_e += 2;
			d_se += 2;
		} else {
			if (cx != cy) {
				_owner->hLine(x - cx, y - cy, x + cx, color);

				if (cy)
					_owner->hLine(x - cx, y + cy, x + cx, color);
			}

			df += d_se;
			d_e += 2;
			d_se += 4;
			cy--;
		}

		cx++;

	} while (cx <= cy);
}

void BITMAP::floodfill(int x, int y, int color) {
	AGS3::floodfill(this, x, y, color);
}

/*-------------------------------------------------------------------*/

/**
 * Dervied screen surface
 */
class Screen : public Graphics::Screen, public BITMAP {
public:
	Screen() : Graphics::Screen(), BITMAP(this) {}
	Screen(int width, int height) : Graphics::Screen(width, height), BITMAP(this) {}
	Screen(int width, int height, const Graphics::PixelFormat &pixelFormat) :
		Graphics::Screen(width, height, pixelFormat), BITMAP(this) {}
	~Screen() override {}
};


/*-------------------------------------------------------------------*/

void set_color_conversion(int mode) {
	color_conversion = mode;
}

int get_color_conversion() {
	return color_conversion;
}

int set_gfx_mode(int card, int w, int h, int v_w, int v_h) {
	// Graphics shutdown can be ignored
	if (card != -1) {
		assert(card == SCUMMVM_ID);
		::AGS::g_vm->setGraphicsMode(w, h);
	}
	return 0;
}

BITMAP *create_bitmap(int width, int height) {
	return new Surface(width, height);
}

BITMAP *create_bitmap_ex(int color_depth, int width, int height) {
	Graphics::PixelFormat format;

	switch (color_depth) {
	case 8:
		format = Graphics::PixelFormat::createFormatCLUT8();
		break;
	case 16:
		format = Graphics::PixelFormat(2, 5, 6, 5, 0, 11, 5, 0, 0);
		break;
	case 32:
		format = Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24);
		break;
	default:
		error("Invalid color depth");
	}

	BITMAP *bitmap = new Surface(width, height, format);
	if (color_depth == 8)
		add_palette_if_needed(bitmap->getSurface());

	return bitmap;
}

BITMAP *create_sub_bitmap(BITMAP *parent, int x, int y, int width, int height) {
	Graphics::ManagedSurface &surf = **parent;
	return new Surface(surf, Common::Rect(x, y, x + width, y + height));
}

BITMAP *create_video_bitmap(int width, int height) {
	return new Screen(width, height);
}

BITMAP *create_system_bitmap(int width, int height) {
	return create_bitmap(width, height);
}

void destroy_bitmap(BITMAP *bitmap) {
	delete bitmap;
}

void set_clip_rect(BITMAP *bitmap, int x1, int y1, int x2, int y2) {
	bitmap->cl = x1;
	bitmap->ct = y1;
	bitmap->cr = x2;
	bitmap->cb = y2;
}

void get_clip_rect(BITMAP *bitmap, int *x1, int *y1, int *x2, int *y2) {
	if (x1)
		*x1 = bitmap->cl;
	if (y1)
		*y1 = bitmap->ct;
	if (x2)
		*x2 = bitmap->cr;
	if (y2)
		*y2 = bitmap->cb;
}

void add_clip_rect(BITMAP *bitmap, int x1, int y1, int x2, int y2) {
	warning("TODO: add_clip_rect");
}

void acquire_bitmap(BITMAP *bitmap) {
	// No implementation needed
}

void release_bitmap(BITMAP *bitmap) {
	// No implementation needed
}

void clear_to_color(BITMAP *bitmap, int color) {
	Graphics::ManagedSurface &surf = **bitmap;

   surf.clear(color);
}

int bitmap_color_depth(BITMAP *bmp) {
	Graphics::ManagedSurface &surf = **bmp;

	return (surf.format.bytesPerPixel == 1) ? 8 : surf.format.bpp();
}

int bitmap_mask_color(BITMAP *bmp) {
	return TRANSPARENT_COLOR(*bmp);
}

void add_palette_if_needed(Graphics::ManagedSurface &surf) {
	if (surf.format.bytesPerPixel == 1) {
		byte pal[PALETTE_SIZE];
		palette_to_rgb8(_current_palette, pal);

		// Set transparent color for index 0
		// fix backgrounds being transparent
		/*pal[0] = 0xff;
		pal[1] = 0;
		pal[2] = 0xff;*/

		surf.setPalette(pal, 0, PALETTE_COUNT);
	}
}

void blit(const BITMAP *src, BITMAP *dest, int src_x, int src_y, int dst_x, int dst_y, int width, int height) {
	Graphics::ManagedSurface &srcS = **src;
	Graphics::ManagedSurface &destS = **dest;

	add_palette_if_needed(srcS);

	if (dynamic_cast<Graphics::Screen *>(&destS) != nullptr) {
		destS.blitFrom(srcS, Common::Rect(src_x, src_y, src_x + width, src_y + height),
			Common::Point(dst_x, dst_y));
	} else {
		destS.rawBlitFrom(srcS, Common::Rect(src_x, src_y, src_x + width, src_y + height),
			Common::Point(dst_x, dst_y), srcS.getPalette());
	}
}

void stretch_blit(const BITMAP *src, BITMAP *dest, int source_x, int source_y, int source_width, int source_height,
		int dest_x, int dest_y, int dest_width, int dest_height) {
	Graphics::ManagedSurface &srcS = **src;
	Graphics::ManagedSurface &destS = **dest;

	add_palette_if_needed(srcS);

	destS.transBlitFrom(srcS, Common::Rect(source_x, source_y, source_x + source_width, source_y + source_height),
		Common::Rect(dest_x, dest_y, dest_x + dest_width, dest_y + dest_height));
}

void masked_blit(const BITMAP *src, BITMAP *dest, int src_x, int src_y, int dst_x, int dst_y, int width, int height) {
	Graphics::ManagedSurface &srcS = **src;
	Graphics::ManagedSurface &destS = **dest;

	add_palette_if_needed(srcS);

	destS.blitFrom(srcS, Common::Rect(src_x, src_y, src_x + width, src_y + height), Common::Point(dst_x, dst_y));
}

void masked_stretch_blit(const BITMAP *src, BITMAP *dest, int source_x, int source_y, int source_width, int source_height,
	int dest_x, int dest_y, int dest_width, int dest_height) {
	Graphics::ManagedSurface &srcS = **src;
	Graphics::ManagedSurface &destS = **dest;

	add_palette_if_needed(srcS);

	destS.transBlitFrom(srcS, Common::Rect(source_x, source_y, source_x + source_width, source_y + source_height),
		Common::Rect(dest_x, dest_y, dest_x + dest_width, dest_y + dest_height));
}

void draw_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y) {
	Graphics::ManagedSurface &bmpS = **bmp;
	Graphics::ManagedSurface &spriteS = **sprite;

	add_palette_if_needed(spriteS);

	bmpS.transBlitFrom(spriteS, Common::Point(x, y), TRANSPARENT_COLOR(spriteS));
}

void stretch_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y, int w, int h) {
	Graphics::ManagedSurface &bmpS = **bmp;
	Graphics::ManagedSurface &spriteS = **sprite;

	add_palette_if_needed(spriteS);

	bmpS.transBlitFrom(spriteS, Common::Rect(0, 0, sprite->w, sprite->h),
		Common::Rect(x, y, x + w, y + h));
}

void draw_trans_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y) {
	bmp->getSurface().blitFrom(sprite->getSurface(), Common::Point(x, y));
}

void draw_lit_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y, int color) {
	// TODO: For now, only 32-bit bitmaps
	assert(sprite->format.bytesPerPixel == 4 && bmp->format.bytesPerPixel == 4);
	byte rSrc, gSrc, bSrc, aSrc;
	byte rDest, gDest, bDest;
	double alpha = (double)color / 255.0;

	for (int yCtr = 0, yp = y; yCtr < sprite->h && yp < bmp->h; ++yCtr, ++yp) {
		if (yp < 0)
			continue;

		const uint32 *srcP = (const uint32 *)sprite->getBasePtr(0, yCtr);
		uint32 *destP = (uint32 *)bmp->getBasePtr(x, yp);

		for (int xCtr = 0, xp = x; xCtr < sprite->w && xp < bmp->w; ++xCtr, ++xp, ++destP) {
			if (x < 0 || x >= bmp->w)
				continue;

			// Get the source and dest pixels
			sprite->format.colorToARGB(*srcP, aSrc, rSrc, gSrc, bSrc);
			bmp->format.colorToRGB(*destP, rDest, gDest, bDest);

			if (rSrc == 255 && gSrc == 0 && bSrc == 255)
				// Skip transparent pixels
				continue;

			// Blend the two
			rDest = static_cast<byte>((rSrc * alpha) + (rDest * (1.0 - alpha)));
			gDest = static_cast<byte>((gSrc * alpha) + (gDest * (1.0 - alpha)));
			bDest = static_cast<byte>((bSrc * alpha) + (bDest * (1.0 - alpha)));

			*destP = bmp->format.RGBToColor(rDest, gDest, bDest);
		}
	}
}

void draw_sprite_h_flip(BITMAP *bmp, const BITMAP *sprite, int x, int y) {
	Graphics::ManagedSurface &bmpS = **bmp;
	Graphics::ManagedSurface &spriteS = **sprite;

	add_palette_if_needed(spriteS);
	bmpS.transBlitFrom(spriteS, Common::Point(x, y), (uint)-1, true);
}

void draw_sprite_v_flip(BITMAP *bmp, const BITMAP *sprite, int x, int y) {
	error("TODO: draw_sprite_v_flip");
}

void draw_sprite_vh_flip(BITMAP *bmp, const BITMAP *sprite, int x, int y) {
	error("TODO: draw_sprite_vh_flip");
}

void rotate_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y, fixed angle) {
	error("TODO: rotate_sprite");
}

void pivot_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y, int cx, int cy, fixed angle) {
	error("TODO: pivot_sprite");
}


bool is_screen_bitmap(BITMAP *bmp) {
	return dynamic_cast<Graphics::Screen *>(bmp) != nullptr;
}

bool is_video_bitmap(BITMAP *bmp) {
	return dynamic_cast<Graphics::Screen *>(bmp) != nullptr;
}

bool is_planar_bitmap(BITMAP *bmp) {
	return false;
}

bool is_linear_bitmap(BITMAP *bmp) {
	return true;
}

void bmp_select(BITMAP *bmp) {
	// No implementation needed
}

byte *bmp_write_line(BITMAP *bmp, int line) {
	return bmp->line[line];
}

void bmp_unwrite_line(BITMAP *bmp) {
	// No implementation needed
}

void bmp_write8(byte *addr, int color) {
	*addr = color;
}

void memory_putpixel(BITMAP *bmp, int x, int y, int color) {
	putpixel(bmp, x, y, color);
}

void putpixel(BITMAP *bmp, int x, int y, int color) {
	Graphics::ManagedSurface &surf = **bmp;
	void *p = surf.getBasePtr(x, y);

	switch (surf.format.bytesPerPixel) {
	case 1:
		*((uint8 *)p) = color;
		break;
	case 2:
		*((uint16 *)p) = color;
		break;
	case 4:
		*((uint32 *)p) = color;
		break;
	default:
		break;
	}
}

void _putpixel(BITMAP *bmp, int x, int y, int color) {
	Graphics::ManagedSurface &surf = **bmp;
	void *p = surf.getBasePtr(x, y);
	*((uint8 *)p) = color;
}

void _putpixel15(BITMAP *bmp, int x, int y, int color) {
	error("Unsupported bpp");
}

void _putpixel16(BITMAP *bmp, int x, int y, int color) {
	Graphics::ManagedSurface &surf = **bmp;
	void *p = surf.getBasePtr(x, y);
	*((uint16 *)p) = color;
}

void _putpixel24(BITMAP *bmp, int x, int y, int color) {
	error("Unsupported bpp");
}

void _putpixel32(BITMAP *bmp, int x, int y, int color) {
	Graphics::ManagedSurface &surf = **bmp;
	void *p = surf.getBasePtr(x, y);
	*((uint32 *)p) = color;
}

int getpixel(BITMAP *bmp, int x, int y) {
	Graphics::ManagedSurface &surf = **bmp;

	// Allegro returns -1 if the pixel lies outside the bitmap
	if (x < 0 || y < 0 || x >= surf.w || y >= surf.h)
		return -1;

	void *p = surf.getBasePtr(x, y);

	switch (surf.format.bytesPerPixel) {
	case 1:
		return *((uint8 *)p);
	case 2:
		return *((uint16 *)p);
	case 4:
		return *((uint32 *)p);
	default:
		break;
	}

	error("Unsupported bpp");
}

int _getpixel(BITMAP *bmp, int x, int y) {
	Graphics::ManagedSurface &surf = **bmp;
	if (x < 0 || y < 0 || x >= surf.w || y >= surf.h)
		return -1;
	void *p = surf.getBasePtr(x, y);
	return *((uint8 *)p);
}

int _getpixel15(BITMAP *bmp, int x, int y) {
	error("Unsupported bpp");
}

int _getpixel16(BITMAP *bmp, int x, int y) {
	Graphics::ManagedSurface &surf = **bmp;
	if (x < 0 || y < 0 || x >= surf.w || y >= surf.h)
		return -1;
	void *p = surf.getBasePtr(x, y);
	return *((uint16 *)p);
}

int _getpixel24(BITMAP *bmp, int x, int y) {
	error("Unsupported bpp");
}

int _getpixel32(BITMAP *bmp, int x, int y) {
	Graphics::ManagedSurface &surf = **bmp;
	if (x < 0 || y < 0 || x >= surf.w || y >= surf.h)
		return -1;
	void *p = surf.getBasePtr(x, y);
	return *((uint32 *)p);
}

void line(BITMAP *bmp, int x1, int y1, int x2, int y2, int color) {
	Graphics::ManagedSurface &surf = **bmp;
	surf.drawLine(x1, y1, x2, y2, color);
}

void rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int color) {
	Graphics::ManagedSurface &surf = **bmp;
	if (x1 > x2)
		SWAP(x1, x2);
	if (y1 > y2)
		SWAP(y1, y2);
	surf.frameRect(Common::Rect(x1, y1, x2, y2), color);
}

void rectfill(BITMAP *bmp, int x1, int y1, int x2, int y2, int color) {
	Graphics::ManagedSurface &surf = **bmp;
	if (x1 > x2)
		SWAP(x1, x2);
	if (y1 > y2)
		SWAP(y1, y2);
	surf.fillRect(Common::Rect(x1, y1, x2, y2), color);
}

void triangle(BITMAP *bmp, int x1, int y1, int x2, int y2, int x3, int y3, int color) {
	Graphics::ManagedSurface &surf = **bmp;
	surf.drawLine(x1, y1, x2, y2, color);
	surf.drawLine(x2, y2, x3, y3, color);
	surf.drawLine(x3, y3, x1, y1, color);
}

void circlefill(BITMAP *bmp, int x, int y, int radius, int color) {
	bmp->circlefill(x, y, radius, color);
}

void clear_bitmap(BITMAP *bmp) {
	bmp->clear();
}

} // namespace AGS3
