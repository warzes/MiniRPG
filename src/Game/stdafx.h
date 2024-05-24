#pragma once

#if defined(_MSC_VER)
#	pragma warning(push, 3)
//#	pragma warning(disable : 5039)
#endif

#include <cassert>
#include <string>
#include <ratio>
#include <algorithm>
#include <filesystem>
#include <random>
#include <chrono>
#include <future>
#include <fstream>
#include <sstream>
#include <variant>
#include <queue>
#include <array>
#include <span>
#include <vector>
#include <unordered_map>

#include "Engine/WindowsInclude.h"
#include <glad/gl.h>

#include <assimp/BaseImporter.h>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/ai_assert.h>
#include <assimp/texture.h>
#include <assimp/Vertex.h>
#include <assimp/Bitmap.h>

#include <glmHeader.h>

#include <reactphysics3d/reactphysics3d.h>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <tiny_gltf.h>
#include <json.h>

#include <json/json.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#include "Engine/Core.h"
#include "Engine/Clock.h"
#include "Engine/EngineTime.h"
#include "Engine/EngineString.h"
#include "Engine/EnumArray.h"
#include "Engine/FileInputStream.h"
#include "Engine/GenerateUUID.h"
#include "Engine/Image.h"
#include "Engine/LogSystem.h"
#include "Engine/MemoryInputStream.h"
#include "Engine/Systems.h"
#include "Engine/Utils.h"
#include "Engine/Color.h"
#include "Engine/Rect.h"
#include "Engine/Vector2.h"
#include "Engine/Vector3.h"
#include "Engine/TempError.h"

#include "Engine/Keyboard.h"
#include "Engine/Mouse.h"
#include "Engine/WindowSystem.h"

#include "Engine/RenderSystem.h"
#include "Engine/RenderCamera.h"

#include "Engine/MainGUISystem.h"

// Audio
#include "Engine/InputSoundFile.h"
#include "Engine/Listener.h"
#include "Engine/Music.h"
#include "Engine/OutputSoundFile.h"
#include "Engine/Sound.h"
#include "Engine/SoundBuffer.h"
#include "Engine/SoundBufferRecorder.h"
#include "Engine/SoundFileFactory.h"
#include "Engine/SoundFileReader.h"
#include "Engine/SoundFileWriter.h"
#include "Engine/SoundRecorder.h"
#include "Engine/SoundSource.h"
#include "Engine/SoundStream.h"

#include "Engine/EngineApp.h"