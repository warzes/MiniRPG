#pragma once

#include "LogSystem.h"

#define OPENGL33 33
#define OPENGL40 40
#define OPENGL41 41
#define OPENGL42 42
#define OPENGL43 43
#define OPENGL44 44
#define OPENGL45 45
#define OPENGL46 46

#define USE_OPENGL_VERSION OPENGL46

namespace OpenGLExtensions
{
	extern uint8_t version;
	extern bool coreDebug;
	extern bool coreDirectStateAccess;
}

namespace Capabilities
{
	extern uint32_t maximumTextureDimension;  // Maximum texture dimension (usually 2048, 4096, 8192 or 16384)
	extern uint32_t maximumTextureUnitCount;
	extern uint32_t maximumUniformBufferSize; // Maximum uniform buffer (UBO) size in bytes (usually at least 4096 *16 bytes, in case there's no support for uniform buffer it's 0)
}

//=============================================================================
// Core enum
//=============================================================================

enum class ImageFormat : uint8_t
{
	None = 0,
	R8,
	RG8,
	RGBA8,
	A8,
#if !PLATFORM_EMSCRIPTEN
	R16,
	RG16,
	RGBA16,
#endif
	R16F,
	RG16F,
	RGBA16F,
	R32F,
	RG32F,
	RGB32F,
	RGBA32F,
	R32U,
	RG32U,
	RGBA32U,
	D16,
#if !PLATFORM_EMSCRIPTEN
	D32,
#endif
	D24S8,

#if !PLATFORM_EMSCRIPTEN
	DXT1,
	DXT3,
	DXT5,
#endif
};

enum class ColorMask : uint8_t
{
	None = 0x00,
	Red = 0x01,
	Green = 0x02,
	Blue = 0x04,
	Alpha = 0x08,
	All = Red | Green | Blue | Alpha
};

//=============================================================================
// Depth Stencil enum
//=============================================================================

enum class ComparisonFunction : uint8_t
{
	Never,
	Equal,
	Less,
	LessEqual,
	Greater,
	GreaterEqual,
	NotEqual,
	Always,
};

enum class StencilOperation : uint8_t
{
	Keep,
	Zero,
	Replace,
	IncrementSaturation,
	DecrementSaturation,
	Invert,
	IncrementWrap,
	DecrementWrap
};

//=============================================================================
// Blend enum
//=============================================================================

enum class BlendOperation : uint8_t
{
	Add,
	Subrtact,
	ReverseSubtract,
	Min,
	Max
};

enum class BlendFactor : uint8_t
{
	Zero,
	One,
	SourceColor,
	InverseSourceColor,
	DestinationColor,
	InverseDestinationColor,
	SourceAlpha,
	InverseSourceAlpha,
	DestinationAlpha,
	InverseDestinationAlpha,
	ConstantColor,
	InverseConstantColor,
	ConstantAlpha,
	InverseConstantAlpha,
	SourceAlphaSaturation,
#if !PLATFORM_EMSCRIPTEN
	Source1Color,
	InverseSource1Color,
	Source1Alpha,
	InverseSource1Alpha
#endif
};

//=============================================================================
// Rasterizer enum
//=============================================================================

enum class RasterizerCullMode : uint8_t
{
	Back,
	Front,
	FrontAndBack
};

enum class FaceOrientation : uint8_t
{
	ClockwiseFace,
	CounterClockwiseFace,
};

enum class RasterizerFillMode : uint8_t
{
	Solid,
	Wireframe,
	Point
};

//=============================================================================
// Shader enum
//=============================================================================

enum class ShaderPipelineStage : uint8_t
{
	Vertex,        // Vertex shader type
	Fragment,      // Fragment shader type (also "Pixel Shader")
#if !PLATFORM_EMSCRIPTEN
	Geometry,      // Geometry shader type
	Compute,       // Compute shader type
	TessControl,   // Tessellation control shader type (also "Hull Shader")
	TessEvaluation // Tessellation evaluation shader type (also "Domain Shader")
#endif // PLATFORM_EMSCRIPTEN
};

//=============================================================================
// GPUBuffer enum
//=============================================================================

enum class BufferTarget : uint8_t
{
	ArrayBuffer,             // GL_ARRAY_BUFFER aka VertexBuffer
#if !PLATFORM_EMSCRIPTEN
	AtomicCounterBuffer,     // GL_ATOMIC_COUNTER_BUFFER
#endif
	CopyReadBuffer,          // GL_COPY_READ_BUFFER
	CopyWriteBuffer,         // GL_COPY_WRITE_BUFFER
#if !PLATFORM_EMSCRIPTEN
	DispatchIndirectBuffer,  // GL_DISPATCH_INDIRECT_BUFFER
	DrawIndirectBuffer,      // GL_DRAW_INDIRECT_BUFFER
#endif
	ElementArrayBuffer,      // GL_ELEMENT_ARRAY_BUFFER aka Index Buffer
	PixelPackBuffer,         // GL_PIXEL_PACK_BUFFER
	PixelUnpackBuffer,       // GL_PIXEL_UNPACK_BUFFER
	QueryBuffer,             // GL_QUERY_BUFFER
#if !PLATFORM_EMSCRIPTEN
	ShaderStorageBuffer,     // GL_SHADER_STORAGE_BUFFER
	TextureBuffer,           // GL_TEXTURE_BUFFER
#endif
	TransformFeedbackBuffer, // GL_TRANSFORM_FEEDBACK_BUFFER
	UniformBuffer            // GL_UNIFORM_BUFFER
};

enum class BufferUsage : uint8_t
{
	StaticDraw,
	DynamicDraw,
	StreamDraw,

	StaticRead,
	DynamicRead,
	StreamRead,

	StaticCopy,
	StreamCopy,
	DynamicCopy
};

#if !PLATFORM_EMSCRIPTEN
enum class BufferMapAccess : uint8_t
{
	Read,
	Write,
	ReadWrite
};
#endif

enum class BufferBinding : uint8_t
{
	ArrayBufferBinding,
#if !PLATFORM_EMSCRIPTEN
	AtomicCounterBufferBinding,
#endif
	CopyReadBuffer,
	CopyWriteBuffer,
#if !PLATFORM_EMSCRIPTEN
	DispatchIndirectBufferBinding,
	DrawIndirectBufferBinding,
#endif
	ElementArrayBufferBinding,
	PixelPackBufferBinding,
	PixelUnpackBufferBinding,
#if !PLATFORM_EMSCRIPTEN
	ShaderStorageBufferBinding,
	TextureBindingBuffer,
#endif
	TransformFeedbackBufferBinding,
	UniformBufferBinding
};

enum class BufferMapAccessFlags : uint8_t
{
	MapReadBit,
	MapWriteBit,
	MapInvalidateRangeBit,
	MapInvalidateBufferBit,
	MapFlushExplicitBit,
	MapUnsynchronizedBit,
#if !PLATFORM_EMSCRIPTEN
	MapPersistentBit,
	MapCoherentBit
#endif
};

//=============================================================================
// IndexBuffer enum
//=============================================================================

enum class IndexFormat : uint8_t
{
	UInt8,
	UInt16,
	UInt32
};

//=============================================================================
// VertexArray enum
//=============================================================================

struct AttribFormat
{
	GLuint attribIndex = 0;
	GLint size = 0;
	GLenum type = 0;
	GLuint relativeOffset = 0;
};

enum class PrimitiveTopology : uint8_t
{
	Points,
	Lines,
	LineStrip,
	LineLoop,
	Triangles,
	TriangleStrip,
	TriangleFan,
#if !PLATFORM_EMSCRIPTEN
	LinesAdjacency,
	LineStripAdjacency,
	TrianglesAdjacency,
	TriangleStripAdjacency,
	Patches
#endif
};

//=============================================================================
// Texture enum
//=============================================================================

enum class TextureType : uint8_t
{
	Texture1D,
	Texture2D,
	Texture3D,
	TextureRectangle,
	Texture1DArray,
	Texture2DArray,
	TextureCubeMap,
	TextureCubeMapArray,
	DepthCubeMap
};

enum class TextureAttachmentType : uint8_t
{
	ColorTexture,
	DepthTexture,
	CubeDepthTexture,
	StencilTexture,
	DepthAndStencilTexture,
	DepthArrayTexture
};

enum class TextureMinFilter : uint8_t
{
	Nearest,
	Linear,
	NearestMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapNearest,
	LinearMipmapLinear,
};

enum class TextureMagFilter : uint8_t
{
	Nearest,
	Linear,
};

enum class TextureAddressMode : uint8_t
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
#if !PLATFORM_EMSCRIPTEN // not support in webGL
	ClampToBorder
#endif
};

enum class TexelsFormat : uint8_t
{
	None = 0,
	R_U8,
	RG_U8,
	RGB_U8,
	RGBA_U8,

	R_F32,
	RG_F32,

	Depth_U16,
	DepthStencil_U16,
	Depth_U24,
	DepthStencil_U24,
};

enum class TextureCubeTarget : uint8_t
{
	TextureCubeMapPositiveX,
	TextureCubeMapNegativeX,
	TextureCubeMapPositiveY,
	TextureCubeMapNegativeY,
	TextureCubeMapPositiveZ,
	TextureCubeMapNegativeZ
};

struct TextureCreateInfo final
{
	GLint internalFormat = GL_RGBA;
	GLenum externalFormat = GL_RGBA;
	GLenum dataType = GL_UNSIGNED_BYTE;
	GLsizei width = 512;
	GLsizei height = 512;
	GLsizei depth = 1;
	std::vector<void*> dataSet;
	GLint type4WrapS = GL_REPEAT;
	GLint type4WrapT = GL_REPEAT;
	GLint type4WrapR = GL_REPEAT;
	GLint type4MinFilter = GL_LINEAR;
	GLint type4MagFilter = GL_LINEAR;
	std::vector<float> borderColor = { 1,1,1,1 };
	bool generateMipMaps = false;
	bool isSRGBSpace = false;
};

//=============================================================================
// Pipeline State Core
//=============================================================================

struct DepthState final
{
	ComparisonFunction compareFunction = ComparisonFunction::Less;
	bool enable = true;
	bool depthWrite = true;
};

struct StencilState final
{
	bool enable = false;
	uint32_t readMask = 0xFFFFFFFFU;
	uint32_t writeMask = 0xFFFFFFFFU;

	ComparisonFunction frontFaceStencilCompareFunction = ComparisonFunction::Always;
	StencilOperation   frontFaceStencilPassOperation = StencilOperation::Keep;  // stencil and depth pass
	StencilOperation   frontFaceStencilFailureOperation = StencilOperation::Keep;  // stencil fail (depth irrelevant)
	StencilOperation   frontFaceStencilDepthFailureOperation = StencilOperation::Keep; // stencil pass, depth fail

	ComparisonFunction backFaceStencilCompareFunction = ComparisonFunction::Always;
	StencilOperation   backFaceStencilPassOperation = StencilOperation::Keep;
	StencilOperation   backFaceStencilFailureOperation = StencilOperation::Keep;
	StencilOperation   backFaceStencilDepthFailureOperation = StencilOperation::Keep;

	int stencilRef = 0;
};

struct DepthStencilState final
{
	DepthState depthState;
	StencilState stencilState;
};

struct BlendState final
{
	BlendFactor colorBlendSource = BlendFactor::One;
	BlendFactor colorBlendDest = BlendFactor::Zero;
	BlendOperation colorOperation = BlendOperation::Add;

	BlendFactor alphaBlendSource = BlendFactor::One;
	BlendFactor alphaBlendDest = BlendFactor::Zero;
	BlendOperation alphaOperation = BlendOperation::Add;

	ColorMask colorMask = ColorMask::All;
	bool enable = false;
};

//=============================================================================
// OpenGLTranslateToGL
//=============================================================================

[[nodiscard]] inline GLenum TranslateToGL(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::R8:              return GL_R8;
	case ImageFormat::RG8:             return GL_RG8;
	case ImageFormat::RGBA8:           return GL_RGBA8;
	case ImageFormat::A8:              return GL_ALPHA;
#if !PLATFORM_EMSCRIPTEN
	case ImageFormat::R16:             return GL_R16;
	case ImageFormat::RG16:            return GL_RG16;
	case ImageFormat::RGBA16:          return GL_RGBA16;
#endif
	case ImageFormat::R16F:            return GL_R16F;
	case ImageFormat::RG16F:           return GL_RG16F;
	case ImageFormat::RGBA16F:         return GL_RGBA16F;
	case ImageFormat::R32F:            return GL_R32F;
	case ImageFormat::RG32F:           return GL_RG32F;
	case ImageFormat::RGB32F:          return GL_RGB32F;
	case ImageFormat::RGBA32F:         return GL_RGBA32F;
	case ImageFormat::R32U:            return GL_R32UI;
	case ImageFormat::RG32U:           return GL_RG32UI;
	case ImageFormat::RGBA32U:         return GL_RGBA32UI;
	case ImageFormat::D16:             return GL_DEPTH_COMPONENT16;
#if !PLATFORM_EMSCRIPTEN
	case ImageFormat::D32:             return GL_DEPTH_COMPONENT32;
#endif
	case ImageFormat::D24S8:           return GL_DEPTH24_STENCIL8;
#if !PLATFORM_EMSCRIPTEN
	case ImageFormat::DXT1:            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	case ImageFormat::DXT3:            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	case ImageFormat::DXT5:            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
#endif
	case ImageFormat::None:
	default: break;
	}
	assert(false && "Unknown ImageFormat");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(ComparisonFunction func)
{
	switch (func)
	{
	case ComparisonFunction::Never:        return GL_NEVER;
	case ComparisonFunction::Equal:        return GL_EQUAL;
	case ComparisonFunction::Less:         return GL_LESS;
	case ComparisonFunction::LessEqual:    return GL_LEQUAL;
	case ComparisonFunction::Greater:      return GL_GREATER;
	case ComparisonFunction::GreaterEqual: return GL_GEQUAL;
	case ComparisonFunction::NotEqual:     return GL_NOTEQUAL;
	case ComparisonFunction::Always:       return GL_ALWAYS;

	default: break;
	}
	assert(false && "Unknown ComparisonFunction");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(StencilOperation op)
{
	switch (op)
	{
	case StencilOperation::Keep:                return GL_KEEP;
	case StencilOperation::Zero:                return GL_ZERO;
	case StencilOperation::Replace:             return GL_REPLACE;
	case StencilOperation::IncrementSaturation: return GL_INCR;
	case StencilOperation::DecrementSaturation: return GL_DECR;
	case StencilOperation::Invert:              return GL_INVERT;
	case StencilOperation::IncrementWrap:       return GL_INCR_WRAP;
	case StencilOperation::DecrementWrap:       return GL_DECR_WRAP;
	default: break;
	}
	assert(false && "Unknown StencilOp");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(BlendOperation op)
{
	switch (op)
	{
	case BlendOperation::Add:             return GL_FUNC_ADD;
	case BlendOperation::Subrtact:        return GL_FUNC_SUBTRACT;
	case BlendOperation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
	case BlendOperation::Min:             return GL_MIN;
	case BlendOperation::Max:             return GL_MAX;
	default: break;
	}
	assert(false && "Unknown BlendOp");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(BlendFactor func)
{
	switch (func)
	{
	case BlendFactor::Zero:                    return GL_ZERO;
	case BlendFactor::One:                     return GL_ONE;
	case BlendFactor::SourceColor:             return GL_SRC_COLOR;
	case BlendFactor::InverseSourceColor:      return GL_ONE_MINUS_SRC_COLOR;
	case BlendFactor::DestinationColor:        return GL_DST_COLOR;
	case BlendFactor::InverseDestinationColor: return GL_ONE_MINUS_DST_COLOR;
	case BlendFactor::SourceAlpha:             return GL_SRC_ALPHA;
	case BlendFactor::InverseSourceAlpha:      return GL_ONE_MINUS_SRC_ALPHA;
	case BlendFactor::DestinationAlpha:        return GL_DST_ALPHA;
	case BlendFactor::InverseDestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
	case BlendFactor::ConstantColor:           return GL_CONSTANT_COLOR;
	case BlendFactor::InverseConstantColor:    return GL_ONE_MINUS_CONSTANT_COLOR;
	case BlendFactor::ConstantAlpha:           return GL_CONSTANT_ALPHA;
	case BlendFactor::InverseConstantAlpha:    return GL_ONE_MINUS_CONSTANT_ALPHA;
	case BlendFactor::SourceAlphaSaturation:   return GL_SRC_ALPHA_SATURATE;
#if !PLATFORM_EMSCRIPTEN
	case BlendFactor::Source1Color:            return GL_SRC1_COLOR;
	case BlendFactor::InverseSource1Color:     return GL_ONE_MINUS_SRC1_COLOR;
	case BlendFactor::Source1Alpha:            return GL_SRC1_ALPHA;
	case BlendFactor::InverseSource1Alpha:     return GL_ONE_MINUS_SRC1_ALPHA;
#endif
	default: break;
	}
	assert(false && "Unknown BlendFactor");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(RasterizerCullMode face)
{
	switch (face)
	{
	case RasterizerCullMode::Back:         return GL_BACK;
	case RasterizerCullMode::Front:        return GL_FRONT;
	case RasterizerCullMode::FrontAndBack: return GL_FRONT_AND_BACK;
	default: break;
	}
	assert(false && "Unknown RasterCullMode");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(FaceOrientation orientation)
{
	switch (orientation)
	{
	case FaceOrientation::ClockwiseFace:        return GL_CW;
	case FaceOrientation::CounterClockwiseFace: return GL_CCW;
	default: break;
	}
	assert(false && "Unknown FaceOrientation");
	return 0;
}

#if !PLATFORM_EMSCRIPTEN
[[nodiscard]] inline GLenum TranslateToGL(RasterizerFillMode fillMode)
{
	switch (fillMode)
	{
	case RasterizerFillMode::Solid:     return GL_FILL;
	case RasterizerFillMode::Wireframe: return GL_LINE;
	case RasterizerFillMode::Point:     return GL_POINT;
	default: break;
	}
	assert(false && "Unknown RasterizerFillMode");
	return 0;
}
#endif

[[nodiscard]] inline GLenum TranslateToGL(ShaderPipelineStage usage)
{
	switch (usage)
	{
	case ShaderPipelineStage::Vertex:         return GL_VERTEX_SHADER;
	case ShaderPipelineStage::Fragment:       return GL_FRAGMENT_SHADER;
#if !PLATFORM_EMSCRIPTEN
	case ShaderPipelineStage::Geometry:       return GL_GEOMETRY_SHADER;
	case ShaderPipelineStage::Compute:        return GL_COMPUTE_SHADER;
	case ShaderPipelineStage::TessControl:    return GL_TESS_CONTROL_SHADER;
	case ShaderPipelineStage::TessEvaluation: return GL_TESS_EVALUATION_SHADER;
#endif // PLATFORM_EMSCRIPTEN
	default: break;
	}
	assert(false && "Unknown ShaderPipelineStage");
	return 0;
}

[[nodiscard]] inline std::string ConvertToStr(ShaderPipelineStage usage)
{
	switch (usage)
	{
	case ShaderPipelineStage::Vertex:         return "Vertex";
	case ShaderPipelineStage::Fragment:       return "Fragment";
#if !PLATFORM_EMSCRIPTEN
	case ShaderPipelineStage::Geometry:       return "Geometry";
	case ShaderPipelineStage::Compute:        return "Compute";
	case ShaderPipelineStage::TessControl:    return "TessControl";
	case ShaderPipelineStage::TessEvaluation: return "TessEvaluation";
#endif // PLATFORM_EMSCRIPTEN
	default: break;
	}
	assert(false && "Unknown ShaderType");
	return "";
}

[[nodiscard]] inline GLenum TranslateToGL(BufferTarget type)
{
	switch (type)
	{
	case BufferTarget::ArrayBuffer:             return GL_ARRAY_BUFFER;
#if !PLATFORM_EMSCRIPTEN
	case BufferTarget::AtomicCounterBuffer:     return GL_ATOMIC_COUNTER_BUFFER;
#endif
	case BufferTarget::CopyReadBuffer:          return GL_COPY_READ_BUFFER;
	case BufferTarget::CopyWriteBuffer:         return GL_COPY_WRITE_BUFFER;
#if !PLATFORM_EMSCRIPTEN
	case BufferTarget::DispatchIndirectBuffer:  return GL_DISPATCH_INDIRECT_BUFFER;
	case BufferTarget::DrawIndirectBuffer:      return GL_DRAW_INDIRECT_BUFFER;
#endif
	case BufferTarget::ElementArrayBuffer:      return GL_ELEMENT_ARRAY_BUFFER;
	case BufferTarget::PixelPackBuffer:         return GL_PIXEL_PACK_BUFFER;
	case BufferTarget::PixelUnpackBuffer:       return GL_PIXEL_UNPACK_BUFFER;
	case BufferTarget::QueryBuffer:             return GL_QUERY_BUFFER;
#if !PLATFORM_EMSCRIPTEN
	case BufferTarget::ShaderStorageBuffer:     return GL_SHADER_STORAGE_BUFFER;
	case BufferTarget::TextureBuffer:           return GL_TEXTURE_BUFFER;
#endif
	case BufferTarget::TransformFeedbackBuffer: return GL_TRANSFORM_FEEDBACK_BUFFER;
	case BufferTarget::UniformBuffer:           return GL_UNIFORM_BUFFER;
	default: break;
	}
	assert(false && "Unknown BufferType");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(BufferUsage usage)
{
	switch (usage)
	{
	case BufferUsage::StaticDraw:  return GL_STATIC_DRAW;
	case BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
	case BufferUsage::StreamDraw:  return GL_STREAM_DRAW;
	case BufferUsage::DynamicRead: return GL_DYNAMIC_READ;
	case BufferUsage::StaticRead:  return GL_STATIC_READ;
	case BufferUsage::StreamRead:  return GL_STREAM_READ;
	case BufferUsage::StaticCopy:  return GL_STATIC_COPY;
	case BufferUsage::DynamicCopy: return GL_DYNAMIC_COPY;
	case BufferUsage::StreamCopy:  return GL_STREAM_COPY;
	default: break;
	}
	assert(false && "Unknown BufferUsage");
	return 0;
}

#if !PLATFORM_EMSCRIPTEN
[[nodiscard]] inline GLenum TranslateToGL(BufferMapAccess access)
{
	switch (access)
	{
	case BufferMapAccess::Read:      return GL_READ_ONLY;
	case BufferMapAccess::Write:     return GL_WRITE_ONLY;
	case BufferMapAccess::ReadWrite: return GL_READ_WRITE;
	default: break;
	}
	assert(false && "Unknown BufferMapAccess");
	return 0;
}
#endif

[[nodiscard]] inline GLenum TranslateToGL(BufferBinding binding)
{
	switch (binding)
	{
	case BufferBinding::ArrayBufferBinding:             return GL_ARRAY_BUFFER_BINDING;
#if !PLATFORM_EMSCRIPTEN
	case BufferBinding::AtomicCounterBufferBinding:     return GL_ATOMIC_COUNTER_BUFFER_BINDING;
#endif
	case BufferBinding::CopyReadBuffer:                 return GL_COPY_READ_BUFFER;
	case BufferBinding::CopyWriteBuffer:                return GL_COPY_WRITE_BUFFER;
#if !PLATFORM_EMSCRIPTEN
	case BufferBinding::DispatchIndirectBufferBinding:  return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
	case BufferBinding::DrawIndirectBufferBinding:      return GL_DRAW_INDIRECT_BUFFER_BINDING;
#endif
	case BufferBinding::ElementArrayBufferBinding:      return GL_ELEMENT_ARRAY_BUFFER_BINDING;
	case BufferBinding::PixelPackBufferBinding:         return GL_PIXEL_PACK_BUFFER_BINDING;
	case BufferBinding::PixelUnpackBufferBinding:       return GL_PIXEL_UNPACK_BUFFER_BINDING;
#if !PLATFORM_EMSCRIPTEN
	case BufferBinding::ShaderStorageBufferBinding:     return GL_SHADER_STORAGE_BUFFER_BINDING;
	case BufferBinding::TextureBindingBuffer:           return GL_TEXTURE_BINDING_BUFFER;
#endif
	case BufferBinding::TransformFeedbackBufferBinding: return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
	case BufferBinding::UniformBufferBinding:           return GL_UNIFORM_BUFFER_BINDING;
	default: break;
	}
	assert(false && "Unknown BufferBinding");
	return 0;
}

[[nodiscard]] inline BufferBinding GetBindingTarget(BufferTarget type)
{
	BufferBinding target;
	switch (type)
	{
	case BufferTarget::ArrayBuffer:             target = BufferBinding::ArrayBufferBinding; break;
#if !PLATFORM_EMSCRIPTEN
	case BufferTarget::AtomicCounterBuffer:     target = BufferBinding::AtomicCounterBufferBinding; break;
#endif
	case BufferTarget::CopyReadBuffer:          target = BufferBinding::CopyReadBuffer; break;
	case BufferTarget::CopyWriteBuffer:         target = BufferBinding::CopyWriteBuffer; break;
#if !PLATFORM_EMSCRIPTEN
	case BufferTarget::DispatchIndirectBuffer:  target = BufferBinding::DispatchIndirectBufferBinding; break;
	case BufferTarget::DrawIndirectBuffer:      target = BufferBinding::DrawIndirectBufferBinding; break;
#endif
	case BufferTarget::ElementArrayBuffer:      target = BufferBinding::ElementArrayBufferBinding; break;
	case BufferTarget::PixelPackBuffer:         target = BufferBinding::PixelPackBufferBinding; break;
	case BufferTarget::PixelUnpackBuffer:       target = BufferBinding::PixelUnpackBufferBinding; break;
#if !PLATFORM_EMSCRIPTEN
	case BufferTarget::ShaderStorageBuffer:     target = BufferBinding::ShaderStorageBufferBinding; break;
	case BufferTarget::TextureBuffer:           target = BufferBinding::TextureBindingBuffer; break;
#endif
	case BufferTarget::TransformFeedbackBuffer: target = BufferBinding::TransformFeedbackBufferBinding; break;
	case BufferTarget::UniformBuffer:           target = BufferBinding::UniformBufferBinding; break;
	default: break;
	}
	assert(false && "Unknown BufferType");
	return {};
}

[[nodiscard]] inline GLenum TranslateToGL(BufferMapAccessFlags accessFlags)
{
	switch (accessFlags)
	{
	case BufferMapAccessFlags::MapReadBit:             return GL_MAP_READ_BIT;
	case BufferMapAccessFlags::MapWriteBit:            return GL_MAP_WRITE_BIT;
	case BufferMapAccessFlags::MapInvalidateRangeBit:  return GL_MAP_INVALIDATE_RANGE_BIT;
	case BufferMapAccessFlags::MapInvalidateBufferBit: return GL_MAP_INVALIDATE_BUFFER_BIT;
	case BufferMapAccessFlags::MapFlushExplicitBit:    return GL_MAP_FLUSH_EXPLICIT_BIT;
	case BufferMapAccessFlags::MapUnsynchronizedBit:   return GL_MAP_UNSYNCHRONIZED_BIT;
#if !PLATFORM_EMSCRIPTEN
	case BufferMapAccessFlags::MapPersistentBit:       return GL_MAP_PERSISTENT_BIT;
	case BufferMapAccessFlags::MapCoherentBit:         return GL_MAP_COHERENT_BIT;
#endif
	default: break;
	}
	assert(false && "Unknown BufferMapAccessFlags");
	return 0;
}

[[nodiscard]] inline constexpr unsigned SizeIndexType(IndexFormat format)
{
	switch (format)
	{
	case IndexFormat::UInt8:  return sizeof(uint8_t);
	case IndexFormat::UInt16: return sizeof(uint16_t);
	case IndexFormat::UInt32: return sizeof(uint32_t);
	default: break;
	}
	assert(false && "Unknown IndexType");
	return 0;
}

[[nodiscard]] inline constexpr unsigned SizeIndexType(unsigned size)
{
	switch (size)
	{
	case 1: return GL_UNSIGNED_BYTE;
	case 2: return GL_UNSIGNED_SHORT;
	case 4: return GL_UNSIGNED_INT;
	default: break;
	}
	assert(false && "Unknown size");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(IndexFormat type)
{
	switch (type)
	{
	case IndexFormat::UInt8:  return GL_UNSIGNED_BYTE;
	case IndexFormat::UInt16: return GL_UNSIGNED_SHORT;
	case IndexFormat::UInt32: return GL_UNSIGNED_INT;
	default: break;
	}
	assert(false && "Unknown IndexType");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(PrimitiveTopology topology)
{
	switch (topology)
	{
	case PrimitiveTopology::Points:                 return GL_POINTS;
	case PrimitiveTopology::Lines:                  return GL_LINES;
	case PrimitiveTopology::LineStrip:              return GL_LINE_STRIP;
	case PrimitiveTopology::LineLoop:               return GL_LINE_LOOP;
	case PrimitiveTopology::Triangles:              return GL_TRIANGLES;
	case PrimitiveTopology::TriangleStrip:          return GL_TRIANGLE_STRIP;
	case PrimitiveTopology::TriangleFan:            return GL_TRIANGLE_FAN;
#if !PLATFORM_EMSCRIPTEN
	case PrimitiveTopology::LinesAdjacency:         return GL_LINES_ADJACENCY;
	case PrimitiveTopology::LineStripAdjacency:     return GL_LINE_STRIP_ADJACENCY;
	case PrimitiveTopology::TrianglesAdjacency:     return GL_TRIANGLES_ADJACENCY;
	case PrimitiveTopology::TriangleStripAdjacency: return GL_TRIANGLE_STRIP_ADJACENCY;
	case PrimitiveTopology::Patches:                return GL_PATCHES;
#endif
	default: break;
	}
	assert(false && "Unknown PrimitiveTopology");
	return 0;
}

[[nodiscard]] inline constexpr GLint TranslateToGL(TextureType type)
{
	switch (type)
	{
	case TextureType::Texture1D:           return GL_TEXTURE_1D;
	case TextureType::Texture2D:           return GL_TEXTURE_2D;
	case TextureType::Texture3D:           return GL_TEXTURE_3D;
	case TextureType::TextureRectangle:    return GL_TEXTURE_RECTANGLE;
	case TextureType::Texture1DArray:      return GL_TEXTURE_1D_ARRAY;
	case TextureType::Texture2DArray:      return GL_TEXTURE_2D_ARRAY;
	case TextureType::TextureCubeMap:      return GL_TEXTURE_CUBE_MAP;
	case TextureType::TextureCubeMapArray: return GL_TEXTURE_CUBE_MAP_ARRAY;
	case TextureType::DepthCubeMap:        return GL_DEPTH_COMPONENT;
	default: break;
	}
	assert(false && "Unknown TextureType");
	return 0;
}

[[nodiscard]] inline GLint TranslateToGL(TextureMinFilter filter)
{
	switch (filter)
	{
	case TextureMinFilter::Nearest:              return GL_NEAREST;
	case TextureMinFilter::Linear:               return GL_LINEAR;
	case TextureMinFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case TextureMinFilter::NearestMipmapLinear:  return GL_NEAREST_MIPMAP_LINEAR;
	case TextureMinFilter::LinearMipmapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
	case TextureMinFilter::LinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
	default: break;
	}
	assert(false && "Unknown TextureMinFilter");
	return 0;
}

[[nodiscard]] inline constexpr GLint TranslateToGL(TextureMagFilter filter)
{
	switch (filter)
	{
	case TextureMagFilter::Nearest: return GL_NEAREST;
	case TextureMagFilter::Linear:  return GL_LINEAR;
	default: break;
	}
	assert(false && "Unknown TextureMagFilter");
	return 0;
}

[[nodiscard]] inline GLint TranslateToGL(TextureAddressMode wrapMode)
{
	switch (wrapMode)
	{
	case TextureAddressMode::Repeat:         return GL_REPEAT;
	case TextureAddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case TextureAddressMode::ClampToEdge:    return GL_CLAMP_TO_EDGE;
#if !PLATFORM_EMSCRIPTEN
	case TextureAddressMode::ClampToBorder:  return GL_CLAMP_TO_BORDER;
#endif
	default: break;
	}
	assert(false && "Unknown TextureAddressMode");
	return 0;
}

[[nodiscard]] inline GLenum TranslateToGL(TextureCubeTarget target)
{
	switch (target)
	{
	case TextureCubeTarget::TextureCubeMapPositiveX: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	case TextureCubeTarget::TextureCubeMapNegativeX: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	case TextureCubeTarget::TextureCubeMapPositiveY: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	case TextureCubeTarget::TextureCubeMapNegativeY: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	case TextureCubeTarget::TextureCubeMapPositiveZ: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
	case TextureCubeTarget::TextureCubeMapNegativeZ: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	default: break;
	}
	assert(false && "Unknown TextureCubeTarget");
	return 0;
}