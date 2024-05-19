#pragma once

#include "LogSystem.h"

namespace GLExtensionsUtils
{
	[[nodiscard]] inline bool CheckExtension(const char* extension)
	{
		// Check whether or not the given extension string pointer is valid
		if (nullptr != extension)
		{
			// Under Windows all available extensions can be received via one additional function but under Linux there are two additional functions for this
#ifdef _WIN32
	// "glGetString()" & "wglGetExtensionsStringARB()"
			const int numberOfLoops = 2;
#elif APPLE
	// On Mac OS X, only "glGetString(GL_EXTENSIONS)" is required
			const int numberOfLoops = 1;
#elif LINUX
	// "glGetString()" & "glXQueryExtensionsString()" & "glXGetClientString()"
			const int numberOfLoops = 3;
#else
#error "Unsupported platform"
#endif
			const char* extensions = nullptr;
			for (int loopIndex = 0; loopIndex < numberOfLoops; ++loopIndex)
			{
				// Extension names should not have spaces
				const char* where = strchr(extension, ' ');
				if (nullptr != where || '\0' == *extension)
				{
					return false; // Extension not found
				}

				// Advanced extensions
				// TODO(sw) Move the query for advanced extensions (via platform specific methods) to the context?
				if (loopIndex > 0)
				{
#ifdef _WIN32
					extensions = static_cast<const char*>(wglGetExtensionsStringARB(wglGetCurrentDC()));
#elif APPLE
					// On Mac OS X, only "glGetString(GL_EXTENSIONS)" is required
#elif LINUX
					// Get the X server display connection
					Display* display = static_cast<OpenGLContextLinux&>(*mOpenGLContext).getDisplay();
					if (nullptr != display)
					{
						if (2 == loopIndex)
						{
							extensions = static_cast<const char*>(glXQueryExtensionsString(display, XDefaultScreen(display)));
						}
						else
						{
							extensions = static_cast<const char*>(glXGetClientString(display, GLX_EXTENSIONS));
						}
					}
#else
#error "Unsupported platform"
#endif

					// Normal extensions
				}
				else
				{
					extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
					if (nullptr == extensions)
					{
						// "glGetString(GL_EXTENSIONS)" is not available in core profiles, we have to use "glGetStringi()" 
						int numberOfExtensions = 0;
						glGetIntegerv(GL_NUM_EXTENSIONS, &numberOfExtensions);
						for (GLuint extensionIndex = 0; extensionIndex < static_cast<GLuint>(numberOfExtensions); ++extensionIndex)
						{
							if (0 == strcmp(extension, reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, extensionIndex))))
							{
								// Extension found
								return true;
							}
						}
					}
				}
				if (nullptr != extensions)
				{
					// It takes a bit of care to be fool-proof about parsing the
					// OpenGL extensions string. Don't be fooled by substrings,
					// etc:
					const char* start = extensions;
					where = strstr(start, extension);
					while (nullptr != where)
					{
						const char* terminator = where + strlen(extension);
						if ((where == start || ' ' == *(where - 1)) && (' ' == *terminator || '\0' == *terminator))
						{
							// Extension found
							return true;
						}
						start = terminator;
						where = strstr(start, extension);
					}
				}
			}
		}

		// Extension not found
		return false;
	}

	[[nodiscard]] inline bool IsSupported(const char* extension)
	{
		if (nullptr != extension)
		{
			if (CheckExtension(extension))
			{
				return true;
			}
		}
		return false;
	}
}

inline bool CheckSupportExtensions()
{
	using namespace GLExtensionsUtils;

	GLint profile = 0;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
	const bool isCoreProfile = (profile & GL_CONTEXT_CORE_PROFILE_BIT);
	if (isCoreProfile == false)
	{
		LogFatal("OPENGL: core profile not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_draw_indirect"))
	{
		LogFatal("OPENGL: GL_ARB_draw_indirect not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_multi_draw_indirect"))
	{
		LogFatal("OPENGL: GL_ARB_multi_draw_indirect not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_separate_shader_objects"))
	{
		LogFatal("OPENGL: GL_ARB_separate_shader_objects not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_get_program_binary"))
	{
		LogFatal("OPENGL: GL_ARB_get_program_binary not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_tessellation_shader"))
	{
		LogFatal("OPENGL: GL_ARB_tessellation_shader not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_geometry_shader4"))
	{
		LogFatal("OPENGL: GL_ARB_geometry_shader4 not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_compute_shader"))
	{
		LogFatal("OPENGL: GL_ARB_compute_shader not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_base_instance"))
	{
		LogFatal("OPENGL: GL_ARB_base_instance not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_direct_state_access"))
	{
		LogFatal("OPENGL: GL_ARB_direct_state_access not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_texture_storage"))
	{
		LogFatal("OPENGL: GL_ARB_texture_storage not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_shader_storage_buffer_object"))
	{
		LogFatal("OPENGL: GL_ARB_shader_storage_buffer_object not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_copy_image"))
	{
		LogFatal("OPENGL: GL_ARB_copy_image not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_gl_spirv"))
	{
		LogFatal("OPENGL: GL_ARB_gl_spirv not support!");
		return false;
	}

	if (!IsSupported("GL_ARB_clip_control"))
	{
		LogFatal("OPENGL: GL_ARB_clip_control not support!");
		return false;
	}

	return true;
}