/* Copyright 2026 TigerVNC Team
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

#ifndef __FLTK_PLATFORM_H__
#define __FLTK_PLATFORM_H__

/*
 * FLTK Platform Header Compatibility Wrapper
 * 
 * This header provides compatibility between FLTK 1.3.x and 1.4.x by
 * including the appropriate platform-specific header based on the FLTK
 * version being used.
 * 
 * FLTK 1.3.x: Uses FL/x.H for platform-specific interfaces
 * FLTK 1.4.x: Deprecates FL/x.H in favor of FL/platform.H
 * 
 * Additionally, this header provides compatibility macros for Wayland
 * support introduced in FLTK 1.4.x, which is not present in 1.3.x.
 */

#include <FL/Fl.H>

// Detect FLTK version and include the appropriate platform header
#if FL_MAJOR_VERSION > 1 || (FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION >= 4)
  // FLTK 1.4.x or later: Use the new platform.H header
  #include <FL/platform.H>
  
  // FLTK 1.4 introduced Wayland support with runtime detection
  #define TIGERVNC_HAVE_FLTK_WAYLAND_DETECTION 1
  
#else
  // FLTK 1.3.x: Use the legacy x.H header
  #include <FL/x.H>
  
  // FLTK 1.3 does not have Wayland support
  #undef TIGERVNC_HAVE_FLTK_WAYLAND_DETECTION
  
  // Provide stub definitions for Wayland detection functions
  // These will always return NULL/false on FLTK 1.3
  #if !defined(WIN32) && !defined(__APPLE__)
    // On Unix platforms, provide compatibility stubs
    inline void* fl_wl_display() { return nullptr; }
  #endif
  
#endif

// Helper macros for platform detection that work on both FLTK versions
#if !defined(WIN32) && !defined(__APPLE__)
  // Unix platform - could be X11 or Wayland (FLTK 1.4 only)
  #define TIGERVNC_UNIX 1
#endif

/*
 * HighDPI Support Detection
 * 
 * FLTK 1.4 introduced comprehensive HighDPI support with automatic scaling.
 * FLTK 1.3 has limited HighDPI support.
 */
#if FL_MAJOR_VERSION > 1 || (FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION >= 4)
  #define TIGERVNC_HAVE_FLTK_HIGHDPI 1
#else
  #undef TIGERVNC_HAVE_FLTK_HIGHDPI
#endif

/*
 * Compatibility helpers for FLTK 1.3 when HighDPI APIs are not available
 */
#ifndef TIGERVNC_HAVE_FLTK_HIGHDPI
  // FLTK 1.3 doesn't have Fl::screen_scale() - assume 1.0
  namespace {
    inline float tigervnc_screen_scale_compat(int /*n*/) {
      return 1.0f;
    }
  }
  #define TIGERVNC_SCREEN_SCALE(n) tigervnc_screen_scale_compat(n)
  
  // FLTK 1.3 doesn't have data_w()/data_h() for images - use w()/h()
  #define TIGERVNC_IMAGE_DATA_WIDTH(img) ((img)->w())
  #define TIGERVNC_IMAGE_DATA_HEIGHT(img) ((img)->h())
#else
  // FLTK 1.4 has Fl::screen_scale()
  #define TIGERVNC_SCREEN_SCALE(n) Fl::screen_scale(n)
  
  // FLTK 1.4 has data_w()/data_h() for actual image pixel dimensions
  #define TIGERVNC_IMAGE_DATA_WIDTH(img) ((img)->data_w())
  #define TIGERVNC_IMAGE_DATA_HEIGHT(img) ((img)->data_h())
#endif

/*
 * Helper functions to get actual pixel dimensions for widgets
 * accounting for HighDPI scaling
 */
#include <FL/Fl_Window.H>

namespace tigervnc {
  // Get the scale factor for a window
  inline float get_window_scale(Fl_Window* win) {
    if (!win) return 1.0f;
#ifdef TIGERVNC_HAVE_FLTK_HIGHDPI
    return TIGERVNC_SCREEN_SCALE(win->screen_num());
#else
    return 1.0f;
#endif
  }
  
  // Get actual pixel width for a widget
  inline int pixel_width(Fl_Widget* widget) {
    if (!widget) return 0;
    Fl_Window* win = widget->window();
    float scale = get_window_scale(win);
    return (int)(widget->w() * scale + 0.5f);
  }
  
  // Get actual pixel height for a widget
  inline int pixel_height(Fl_Widget* widget) {
    if (!widget) return 0;
    Fl_Window* win = widget->window();
    float scale = get_window_scale(win);
    return (int)(widget->h() * scale + 0.5f);
  }
}

#endif // __FLTK_PLATFORM_H__
