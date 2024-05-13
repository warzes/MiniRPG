/**
 * Loader generated by glad 2.0.6 on Sat May 11 13:04:25 2024
 *
 * SPDX-License-Identifier: (WTFPL OR CC0-1.0) AND Apache-2.0
 *
 * Generator: C/C++
 * Specification: wgl
 * Extensions: 17
 *
 * APIs:
 *  - wgl=1.0
 *
 * Options:
 *  - ALIAS = False
 *  - DEBUG = False
 *  - HEADER_ONLY = False
 *  - LOADER = True
 *  - MX = False
 *  - ON_DEMAND = False
 *
 * Commandline:
 *    --api='wgl=1.0' --extensions='WGL_ARB_buffer_region,WGL_ARB_context_flush_control,WGL_ARB_create_context,WGL_ARB_create_context_no_error,WGL_ARB_create_context_profile,WGL_ARB_create_context_robustness,WGL_ARB_extensions_string,WGL_ARB_framebuffer_sRGB,WGL_ARB_make_current_read,WGL_ARB_multisample,WGL_ARB_pbuffer,WGL_ARB_pixel_format,WGL_ARB_pixel_format_float,WGL_ARB_render_texture,WGL_ARB_robustness_application_isolation,WGL_ARB_robustness_share_group_isolation,WGL_EXT_extensions_string' c --loader
 *
 * Online:
 *    http://glad.sh/#api=wgl%3D1.0&extensions=WGL_ARB_buffer_region%2CWGL_ARB_context_flush_control%2CWGL_ARB_create_context%2CWGL_ARB_create_context_no_error%2CWGL_ARB_create_context_profile%2CWGL_ARB_create_context_robustness%2CWGL_ARB_extensions_string%2CWGL_ARB_framebuffer_sRGB%2CWGL_ARB_make_current_read%2CWGL_ARB_multisample%2CWGL_ARB_pbuffer%2CWGL_ARB_pixel_format%2CWGL_ARB_pixel_format_float%2CWGL_ARB_render_texture%2CWGL_ARB_robustness_application_isolation%2CWGL_ARB_robustness_share_group_isolation%2CWGL_EXT_extensions_string&generator=c&options=LOADER
 *
 */

#ifndef GLAD_WGL_H_
#define GLAD_WGL_H_

#include <windows.h>
#include <glad/gl.h>

#define GLAD_WGL
#define GLAD_OPTION_WGL_LOADER

#ifdef __cplusplus
extern "C" {
#endif

#define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054
#define ERROR_INVALID_PIXEL_TYPE_ARB 0x2043
#define ERROR_INVALID_PROFILE_ARB 0x2096
#define ERROR_INVALID_VERSION_ARB 0x2095
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
#define WGL_ACCUM_BITS_ARB 0x201D
#define WGL_ACCUM_BLUE_BITS_ARB 0x2020
#define WGL_ACCUM_GREEN_BITS_ARB 0x201F
#define WGL_ACCUM_RED_BITS_ARB 0x201E
#define WGL_ALPHA_BITS_ARB 0x201B
#define WGL_ALPHA_SHIFT_ARB 0x201C
#define WGL_AUX0_ARB 0x2087
#define WGL_AUX1_ARB 0x2088
#define WGL_AUX2_ARB 0x2089
#define WGL_AUX3_ARB 0x208A
#define WGL_AUX4_ARB 0x208B
#define WGL_AUX5_ARB 0x208C
#define WGL_AUX6_ARB 0x208D
#define WGL_AUX7_ARB 0x208E
#define WGL_AUX8_ARB 0x208F
#define WGL_AUX9_ARB 0x2090
#define WGL_AUX_BUFFERS_ARB 0x2024
#define WGL_BACK_COLOR_BUFFER_BIT_ARB 0x00000002
#define WGL_BACK_LEFT_ARB 0x2085
#define WGL_BACK_RIGHT_ARB 0x2086
#define WGL_BIND_TO_TEXTURE_RGBA_ARB 0x2071
#define WGL_BIND_TO_TEXTURE_RGB_ARB 0x2070
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_BLUE_SHIFT_ARB 0x201A
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_OPENGL_NO_ERROR_ARB 0x31B3
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0
#define WGL_CONTEXT_RESET_ISOLATION_BIT_ARB 0x00000008
#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
#define WGL_CUBE_MAP_FACE_ARB 0x207C
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_DEPTH_BUFFER_BIT_ARB 0x00000004
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_DRAW_TO_BITMAP_ARB 0x2002
#define WGL_DRAW_TO_PBUFFER_ARB 0x202D
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_FONT_LINES 0
#define WGL_FONT_POLYGONS 1
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20A9
#define WGL_FRONT_COLOR_BUFFER_BIT_ARB 0x00000001
#define WGL_FRONT_LEFT_ARB 0x2083
#define WGL_FRONT_RIGHT_ARB 0x2084
#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_GENERIC_ACCELERATION_ARB 0x2026
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_GREEN_SHIFT_ARB 0x2018
#define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define WGL_MAX_PBUFFER_HEIGHT_ARB 0x2030
#define WGL_MAX_PBUFFER_PIXELS_ARB 0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB 0x202F
#define WGL_MIPMAP_LEVEL_ARB 0x207B
#define WGL_MIPMAP_TEXTURE_ARB 0x2074
#define WGL_NEED_PALETTE_ARB 0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB 0x2005
#define WGL_NO_ACCELERATION_ARB 0x2025
#define WGL_NO_RESET_NOTIFICATION_ARB 0x8261
#define WGL_NO_TEXTURE_ARB 0x2077
#define WGL_NUMBER_OVERLAYS_ARB 0x2008
#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
#define WGL_NUMBER_UNDERLAYS_ARB 0x2009
#define WGL_PBUFFER_HEIGHT_ARB 0x2035
#define WGL_PBUFFER_LARGEST_ARB 0x2033
#define WGL_PBUFFER_LOST_ARB 0x2036
#define WGL_PBUFFER_WIDTH_ARB 0x2034
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_RED_BITS_ARB 0x2015
#define WGL_RED_SHIFT_ARB 0x2016
#define WGL_SAMPLES_ARB 0x2042
#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SHARE_ACCUM_ARB 0x200E
#define WGL_SHARE_DEPTH_ARB 0x200C
#define WGL_SHARE_STENCIL_ARB 0x200D
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_STENCIL_BUFFER_BIT_ARB 0x00000008
#define WGL_STEREO_ARB 0x2012
#define WGL_SUPPORT_GDI_ARB 0x200F
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_SWAP_COPY_ARB 0x2029
#define WGL_SWAP_EXCHANGE_ARB 0x2028
#define WGL_SWAP_LAYER_BUFFERS_ARB 0x2006
#define WGL_SWAP_MAIN_PLANE 0x00000001
#define WGL_SWAP_METHOD_ARB 0x2007
#define WGL_SWAP_OVERLAY1 0x00000002
#define WGL_SWAP_OVERLAY10 0x00000400
#define WGL_SWAP_OVERLAY11 0x00000800
#define WGL_SWAP_OVERLAY12 0x00001000
#define WGL_SWAP_OVERLAY13 0x00002000
#define WGL_SWAP_OVERLAY14 0x00004000
#define WGL_SWAP_OVERLAY15 0x00008000
#define WGL_SWAP_OVERLAY2 0x00000004
#define WGL_SWAP_OVERLAY3 0x00000008
#define WGL_SWAP_OVERLAY4 0x00000010
#define WGL_SWAP_OVERLAY5 0x00000020
#define WGL_SWAP_OVERLAY6 0x00000040
#define WGL_SWAP_OVERLAY7 0x00000080
#define WGL_SWAP_OVERLAY8 0x00000100
#define WGL_SWAP_OVERLAY9 0x00000200
#define WGL_SWAP_UNDEFINED_ARB 0x202A
#define WGL_SWAP_UNDERLAY1 0x00010000
#define WGL_SWAP_UNDERLAY10 0x02000000
#define WGL_SWAP_UNDERLAY11 0x04000000
#define WGL_SWAP_UNDERLAY12 0x08000000
#define WGL_SWAP_UNDERLAY13 0x10000000
#define WGL_SWAP_UNDERLAY14 0x20000000
#define WGL_SWAP_UNDERLAY15 0x40000000
#define WGL_SWAP_UNDERLAY2 0x00020000
#define WGL_SWAP_UNDERLAY3 0x00040000
#define WGL_SWAP_UNDERLAY4 0x00080000
#define WGL_SWAP_UNDERLAY5 0x00100000
#define WGL_SWAP_UNDERLAY6 0x00200000
#define WGL_SWAP_UNDERLAY7 0x00400000
#define WGL_SWAP_UNDERLAY8 0x00800000
#define WGL_SWAP_UNDERLAY9 0x01000000
#define WGL_TEXTURE_1D_ARB 0x2079
#define WGL_TEXTURE_2D_ARB 0x207A
#define WGL_TEXTURE_CUBE_MAP_ARB 0x2078
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x207E
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x2080
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x2082
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x207D
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x207F
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x2081
#define WGL_TEXTURE_FORMAT_ARB 0x2072
#define WGL_TEXTURE_RGBA_ARB 0x2076
#define WGL_TEXTURE_RGB_ARB 0x2075
#define WGL_TEXTURE_TARGET_ARB 0x2073
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB 0x203A
#define WGL_TRANSPARENT_ARB 0x200A
#define WGL_TRANSPARENT_BLUE_VALUE_ARB 0x2039
#define WGL_TRANSPARENT_GREEN_VALUE_ARB 0x2038
#define WGL_TRANSPARENT_INDEX_VALUE_ARB 0x203B
#define WGL_TRANSPARENT_RED_VALUE_ARB 0x2037
#define WGL_TYPE_COLORINDEX_ARB 0x202C
#define WGL_TYPE_RGBA_ARB 0x202B
#define WGL_TYPE_RGBA_FLOAT_ARB 0x21A0

struct _GPU_DEVICE {
    DWORD  cb;
    CHAR   DeviceName[32];
    CHAR   DeviceString[128];
    DWORD  Flags;
    RECT   rcVirtualScreen;
};
DECLARE_HANDLE(HPBUFFERARB);
DECLARE_HANDLE(HPBUFFEREXT);
DECLARE_HANDLE(HVIDEOOUTPUTDEVICENV);
DECLARE_HANDLE(HPVIDEODEV);
DECLARE_HANDLE(HPGPUNV);
DECLARE_HANDLE(HGPUNV);
DECLARE_HANDLE(HVIDEOINPUTDEVICENV);
typedef struct _GPU_DEVICE GPU_DEVICE;
typedef struct _GPU_DEVICE *PGPU_DEVICE;

#define WGL_VERSION_1_0 1
GLAD_API_CALL int GLAD_WGL_VERSION_1_0;
#define WGL_ARB_buffer_region 1
GLAD_API_CALL int GLAD_WGL_ARB_buffer_region;
#define WGL_ARB_context_flush_control 1
GLAD_API_CALL int GLAD_WGL_ARB_context_flush_control;
#define WGL_ARB_create_context 1
GLAD_API_CALL int GLAD_WGL_ARB_create_context;
#define WGL_ARB_create_context_no_error 1
GLAD_API_CALL int GLAD_WGL_ARB_create_context_no_error;
#define WGL_ARB_create_context_profile 1
GLAD_API_CALL int GLAD_WGL_ARB_create_context_profile;
#define WGL_ARB_create_context_robustness 1
GLAD_API_CALL int GLAD_WGL_ARB_create_context_robustness;
#define WGL_ARB_extensions_string 1
GLAD_API_CALL int GLAD_WGL_ARB_extensions_string;
#define WGL_ARB_framebuffer_sRGB 1
GLAD_API_CALL int GLAD_WGL_ARB_framebuffer_sRGB;
#define WGL_ARB_make_current_read 1
GLAD_API_CALL int GLAD_WGL_ARB_make_current_read;
#define WGL_ARB_multisample 1
GLAD_API_CALL int GLAD_WGL_ARB_multisample;
#define WGL_ARB_pbuffer 1
GLAD_API_CALL int GLAD_WGL_ARB_pbuffer;
#define WGL_ARB_pixel_format 1
GLAD_API_CALL int GLAD_WGL_ARB_pixel_format;
#define WGL_ARB_pixel_format_float 1
GLAD_API_CALL int GLAD_WGL_ARB_pixel_format_float;
#define WGL_ARB_render_texture 1
GLAD_API_CALL int GLAD_WGL_ARB_render_texture;
#define WGL_ARB_robustness_application_isolation 1
GLAD_API_CALL int GLAD_WGL_ARB_robustness_application_isolation;
#define WGL_ARB_robustness_share_group_isolation 1
GLAD_API_CALL int GLAD_WGL_ARB_robustness_share_group_isolation;
#define WGL_EXT_extensions_string 1
GLAD_API_CALL int GLAD_WGL_EXT_extensions_string;


typedef int (GLAD_API_PTR *PFNCHOOSEPIXELFORMATPROC)(HDC hDc, const PIXELFORMATDESCRIPTOR * pPfd);
typedef int (GLAD_API_PTR *PFNDESCRIBEPIXELFORMATPROC)(HDC hdc, int ipfd, UINT cjpfd, PIXELFORMATDESCRIPTOR * ppfd);
typedef UINT (GLAD_API_PTR *PFNGETENHMETAFILEPIXELFORMATPROC)(HENHMETAFILE hemf, UINT cbBuffer, PIXELFORMATDESCRIPTOR * ppfd);
typedef int (GLAD_API_PTR *PFNGETPIXELFORMATPROC)(HDC hdc);
typedef BOOL (GLAD_API_PTR *PFNSETPIXELFORMATPROC)(HDC hdc, int ipfd, const PIXELFORMATDESCRIPTOR * ppfd);
typedef BOOL (GLAD_API_PTR *PFNSWAPBUFFERSPROC)(HDC hdc);
typedef BOOL (GLAD_API_PTR *PFNWGLBINDTEXIMAGEARBPROC)(HPBUFFERARB hPbuffer, int iBuffer);
typedef BOOL (GLAD_API_PTR *PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats);
typedef BOOL (GLAD_API_PTR *PFNWGLCOPYCONTEXTPROC)(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask);
typedef HANDLE (GLAD_API_PTR *PFNWGLCREATEBUFFERREGIONARBPROC)(HDC hDC, int iLayerPlane, UINT uType);
typedef HGLRC (GLAD_API_PTR *PFNWGLCREATECONTEXTPROC)(HDC hDc);
typedef HGLRC (GLAD_API_PTR *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int * attribList);
typedef HGLRC (GLAD_API_PTR *PFNWGLCREATELAYERCONTEXTPROC)(HDC hDc, int level);
typedef HPBUFFERARB (GLAD_API_PTR *PFNWGLCREATEPBUFFERARBPROC)(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList);
typedef VOID (GLAD_API_PTR *PFNWGLDELETEBUFFERREGIONARBPROC)(HANDLE hRegion);
typedef BOOL (GLAD_API_PTR *PFNWGLDELETECONTEXTPROC)(HGLRC oldContext);
typedef BOOL (GLAD_API_PTR *PFNWGLDESCRIBELAYERPLANEPROC)(HDC hDc, int pixelFormat, int layerPlane, UINT nBytes, LAYERPLANEDESCRIPTOR * plpd);
typedef BOOL (GLAD_API_PTR *PFNWGLDESTROYPBUFFERARBPROC)(HPBUFFERARB hPbuffer);
typedef HGLRC (GLAD_API_PTR *PFNWGLGETCURRENTCONTEXTPROC)(void);
typedef HDC (GLAD_API_PTR *PFNWGLGETCURRENTDCPROC)(void);
typedef HDC (GLAD_API_PTR *PFNWGLGETCURRENTREADDCARBPROC)(void);
typedef const char * (GLAD_API_PTR *PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);
typedef const char * (GLAD_API_PTR *PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
typedef int (GLAD_API_PTR *PFNWGLGETLAYERPALETTEENTRIESPROC)(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF * pcr);
typedef HDC (GLAD_API_PTR *PFNWGLGETPBUFFERDCARBPROC)(HPBUFFERARB hPbuffer);
typedef BOOL (GLAD_API_PTR *PFNWGLGETPIXELFORMATATTRIBFVARBPROC)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int * piAttributes, FLOAT * pfValues);
typedef BOOL (GLAD_API_PTR *PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int * piAttributes, int * piValues);
typedef PROC (GLAD_API_PTR *PFNWGLGETPROCADDRESSPROC)(LPCSTR lpszProc);
typedef BOOL (GLAD_API_PTR *PFNWGLMAKECONTEXTCURRENTARBPROC)(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
typedef BOOL (GLAD_API_PTR *PFNWGLMAKECURRENTPROC)(HDC hDc, HGLRC newContext);
typedef BOOL (GLAD_API_PTR *PFNWGLQUERYPBUFFERARBPROC)(HPBUFFERARB hPbuffer, int iAttribute, int * piValue);
typedef BOOL (GLAD_API_PTR *PFNWGLREALIZELAYERPALETTEPROC)(HDC hdc, int iLayerPlane, BOOL bRealize);
typedef int (GLAD_API_PTR *PFNWGLRELEASEPBUFFERDCARBPROC)(HPBUFFERARB hPbuffer, HDC hDC);
typedef BOOL (GLAD_API_PTR *PFNWGLRELEASETEXIMAGEARBPROC)(HPBUFFERARB hPbuffer, int iBuffer);
typedef BOOL (GLAD_API_PTR *PFNWGLRESTOREBUFFERREGIONARBPROC)(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc);
typedef BOOL (GLAD_API_PTR *PFNWGLSAVEBUFFERREGIONARBPROC)(HANDLE hRegion, int x, int y, int width, int height);
typedef int (GLAD_API_PTR *PFNWGLSETLAYERPALETTEENTRIESPROC)(HDC hdc, int iLayerPlane, int iStart, int cEntries, const COLORREF * pcr);
typedef BOOL (GLAD_API_PTR *PFNWGLSETPBUFFERATTRIBARBPROC)(HPBUFFERARB hPbuffer, const int * piAttribList);
typedef BOOL (GLAD_API_PTR *PFNWGLSHARELISTSPROC)(HGLRC hrcSrvShare, HGLRC hrcSrvSource);
typedef BOOL (GLAD_API_PTR *PFNWGLSWAPLAYERBUFFERSPROC)(HDC hdc, UINT fuFlags);
typedef BOOL (GLAD_API_PTR *PFNWGLUSEFONTBITMAPSPROC)(HDC hDC, DWORD first, DWORD count, DWORD listBase);
typedef BOOL (GLAD_API_PTR *PFNWGLUSEFONTBITMAPSAPROC)(HDC hDC, DWORD first, DWORD count, DWORD listBase);
typedef BOOL (GLAD_API_PTR *PFNWGLUSEFONTBITMAPSWPROC)(HDC hDC, DWORD first, DWORD count, DWORD listBase);
typedef BOOL (GLAD_API_PTR *PFNWGLUSEFONTOUTLINESPROC)(HDC hDC, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf);
typedef BOOL (GLAD_API_PTR *PFNWGLUSEFONTOUTLINESAPROC)(HDC hDC, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf);
typedef BOOL (GLAD_API_PTR *PFNWGLUSEFONTOUTLINESWPROC)(HDC hDC, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf);

GLAD_API_CALL PFNWGLBINDTEXIMAGEARBPROC glad_wglBindTexImageARB;
#define wglBindTexImageARB glad_wglBindTexImageARB
GLAD_API_CALL PFNWGLCHOOSEPIXELFORMATARBPROC glad_wglChoosePixelFormatARB;
#define wglChoosePixelFormatARB glad_wglChoosePixelFormatARB
GLAD_API_CALL PFNWGLCREATEBUFFERREGIONARBPROC glad_wglCreateBufferRegionARB;
#define wglCreateBufferRegionARB glad_wglCreateBufferRegionARB
GLAD_API_CALL PFNWGLCREATECONTEXTATTRIBSARBPROC glad_wglCreateContextAttribsARB;
#define wglCreateContextAttribsARB glad_wglCreateContextAttribsARB
GLAD_API_CALL PFNWGLCREATEPBUFFERARBPROC glad_wglCreatePbufferARB;
#define wglCreatePbufferARB glad_wglCreatePbufferARB
GLAD_API_CALL PFNWGLDELETEBUFFERREGIONARBPROC glad_wglDeleteBufferRegionARB;
#define wglDeleteBufferRegionARB glad_wglDeleteBufferRegionARB
GLAD_API_CALL PFNWGLDESTROYPBUFFERARBPROC glad_wglDestroyPbufferARB;
#define wglDestroyPbufferARB glad_wglDestroyPbufferARB
GLAD_API_CALL PFNWGLGETCURRENTREADDCARBPROC glad_wglGetCurrentReadDCARB;
#define wglGetCurrentReadDCARB glad_wglGetCurrentReadDCARB
GLAD_API_CALL PFNWGLGETEXTENSIONSSTRINGARBPROC glad_wglGetExtensionsStringARB;
#define wglGetExtensionsStringARB glad_wglGetExtensionsStringARB
GLAD_API_CALL PFNWGLGETEXTENSIONSSTRINGEXTPROC glad_wglGetExtensionsStringEXT;
#define wglGetExtensionsStringEXT glad_wglGetExtensionsStringEXT
GLAD_API_CALL PFNWGLGETPBUFFERDCARBPROC glad_wglGetPbufferDCARB;
#define wglGetPbufferDCARB glad_wglGetPbufferDCARB
GLAD_API_CALL PFNWGLGETPIXELFORMATATTRIBFVARBPROC glad_wglGetPixelFormatAttribfvARB;
#define wglGetPixelFormatAttribfvARB glad_wglGetPixelFormatAttribfvARB
GLAD_API_CALL PFNWGLGETPIXELFORMATATTRIBIVARBPROC glad_wglGetPixelFormatAttribivARB;
#define wglGetPixelFormatAttribivARB glad_wglGetPixelFormatAttribivARB
GLAD_API_CALL PFNWGLMAKECONTEXTCURRENTARBPROC glad_wglMakeContextCurrentARB;
#define wglMakeContextCurrentARB glad_wglMakeContextCurrentARB
GLAD_API_CALL PFNWGLQUERYPBUFFERARBPROC glad_wglQueryPbufferARB;
#define wglQueryPbufferARB glad_wglQueryPbufferARB
GLAD_API_CALL PFNWGLRELEASEPBUFFERDCARBPROC glad_wglReleasePbufferDCARB;
#define wglReleasePbufferDCARB glad_wglReleasePbufferDCARB
GLAD_API_CALL PFNWGLRELEASETEXIMAGEARBPROC glad_wglReleaseTexImageARB;
#define wglReleaseTexImageARB glad_wglReleaseTexImageARB
GLAD_API_CALL PFNWGLRESTOREBUFFERREGIONARBPROC glad_wglRestoreBufferRegionARB;
#define wglRestoreBufferRegionARB glad_wglRestoreBufferRegionARB
GLAD_API_CALL PFNWGLSAVEBUFFERREGIONARBPROC glad_wglSaveBufferRegionARB;
#define wglSaveBufferRegionARB glad_wglSaveBufferRegionARB
GLAD_API_CALL PFNWGLSETPBUFFERATTRIBARBPROC glad_wglSetPbufferAttribARB;
#define wglSetPbufferAttribARB glad_wglSetPbufferAttribARB

GLAD_API_CALL int gladLoadWGLUserPtr(HDC hdc, GLADuserptrloadfunc load, void *userptr);
GLAD_API_CALL int gladLoadWGL(HDC hdc, GLADloadfunc load);

#ifdef GLAD_WGL

GLAD_API_CALL int gladLoaderLoadWGL(HDC hdc);

#endif
#ifdef __cplusplus
}
#endif
#endif
