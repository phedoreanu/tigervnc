/* Copyright 2025 Pierre Ossman for Cendio AB
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Image_Surface.H>
#include <FL/fl_draw.H>
#include <FL/Fl_RGB_Image.H>

#ifdef __unix__
#include <FL/x.H>
#endif

#include <gtest/gtest.h>

// Helper function to detect if we can use the display
// This is needed for headless CI environments
static bool canUseDisplay()
{
#ifdef __unix__
  // Check if we have a display available
  if (fl_display == nullptr) {
    fl_open_display();
  }
  return fl_display != nullptr;
#else
  // On Windows and macOS, assume display is always available
  return true;
#endif
}

// Test FLTK version detection
TEST(FltkTest, VersionDetection)
{
  EXPECT_EQ(FL_MAJOR_VERSION, 1);
  EXPECT_TRUE(FL_MINOR_VERSION == 3 || FL_MINOR_VERSION == 4);
}

// Test Fl_Image_Surface creation and usage
TEST(FltkTest, ImageSurface)
{
  if (!canUseDisplay()) {
    GTEST_SKIP() << "Skipping test because no display is available (headless environment)";
  }

  Fl_Image_Surface *surface = new Fl_Image_Surface(100, 100);
  ASSERT_NE(surface, nullptr);
  
  surface->set_current();
  fl_color(FL_RED);
  fl_rectf(0, 0, 100, 100);
  
  Fl_RGB_Image *image = surface->image();
  ASSERT_NE(image, nullptr);
  EXPECT_EQ(image->w(), 100);
  EXPECT_EQ(image->h(), 100);
  
  delete image;
  delete surface;
  Fl_Display_Device::display_device()->set_current();
}

// Test basic drawing primitives
TEST(FltkTest, DrawingPrimitives)
{
  if (!canUseDisplay()) {
    GTEST_SKIP() << "Skipping test because no display is available (headless environment)";
  }

  Fl_Image_Surface *surface = new Fl_Image_Surface(100, 100);
  surface->set_current();
  
  // Test various drawing functions
  fl_color(FL_BLACK);
  fl_rectf(0, 0, 100, 100);
  
  fl_color(FL_WHITE);
  fl_line(0, 0, 100, 100);
  fl_rect(10, 10, 80, 80);
  fl_point(50, 50);
  fl_circle(50.0, 50.0, 20.0);
  fl_arc(30, 30, 40, 40, 0.0, 360.0);
  
  delete surface;
  Fl_Display_Device::display_device()->set_current();
}

// Test color functions
TEST(FltkTest, ColorFunctions)
{
  // Test color conversion functions
  fl_color(FL_RED);
  fl_color(fl_rgb_color(255, 0, 0));
  
  unsigned char r, g, b;
  Fl::get_color(FL_RED, r, g, b);
  // Red should have non-zero red component
  EXPECT_GT(r, 0);
}

// Test clipping
TEST(FltkTest, Clipping)
{
  if (!canUseDisplay()) {
    GTEST_SKIP() << "Skipping test because no display is available (headless environment)";
  }

  Fl_Image_Surface *surface = new Fl_Image_Surface(100, 100);
  surface->set_current();
  
  fl_push_clip(10, 10, 80, 80);
  fl_color(FL_RED);
  fl_rectf(0, 0, 100, 100);
  fl_pop_clip();
  
  delete surface;
  Fl_Display_Device::display_device()->set_current();
}

// Test Fl_RGB_Image
TEST(FltkTest, RGBImage)
{
  const int w = 10, h = 10;
  unsigned char *data = new unsigned char[w * h * 3];
  
  // Fill with red
  for (int i = 0; i < w * h * 3; i += 3) {
    data[i] = 255;     // R
    data[i+1] = 0;     // G
    data[i+2] = 0;     // B
  }
  
  Fl_RGB_Image *image = new Fl_RGB_Image(data, w, h, 3);
  ASSERT_NE(image, nullptr);
  EXPECT_EQ(image->w(), w);
  EXPECT_EQ(image->h(), h);
  EXPECT_EQ(image->d(), 3);
  
  delete image;
  delete[] data;
}

// Test screen scaling awareness (FLTK 1.4+ feature)
#if FL_MINOR_VERSION >= 4
TEST(FltkTest, ScreenScaling)
{
  if (!canUseDisplay()) {
    GTEST_SKIP() << "Skipping test because no display is available (headless environment)";
  }

  float scale = Fl::screen_scale(0);
  EXPECT_GT(scale, 0.0f);
  EXPECT_LE(scale, 10.0f); // Sanity check
}
#endif
