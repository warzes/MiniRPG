#pragma once

#if defined(_MSC_VER)
#	pragma warning(push, 3)
//#	pragma warning(disable : 5039)
#endif

#include <cassert>
#include <string>
#include <random>
#include <ratio>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <fstream>
#include <sstream>
#include <variant>
#include <queue>
#include <array>
#include <span>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "WindowsInclude.h"
#include <mmsystem.h>

#include <glad/gl.h>
#include <glad/wgl.h>

#include <glmHeader.h>

#include <stb/stb_image.h>

//#define OV_EXCLUDE_STATIC_CALLBACKS
#define FLAC__NO_DLL
//#define MA_NO_MP3
//#define MA_NO_FLAC
//#define MA_NO_ENCODING
//#define MA_NO_RESOURCE_MANAGER
//#define MA_NO_GENERATION

#include <miniaudio.h>
#include <FLAC/stream_decoder.h>
#include <FLAC/stream_encoder.h>
#define MINIMP3_NO_STDIO // Minimp3 control define, eliminate file manipulation code which is useless here
#include <minimp3/minimp3_ex.h>
#undef MINIMP3_NO_STDIO
#include <vorbis/vorbisfile.h>
#include <vorbis/vorbisenc.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#include "Core.h"