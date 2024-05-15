#include "stdafx.h"
#include "Shader.h"

// Shader code
const char* blurFragSource =
#include "blur.frag.glsl"
;

const char* blurVertSource =
#include "blur.vert.glsl"
;

const char* gbufferFragSource =
#include "gbuffer.frag.glsl"
;

const char* gbufferVertSource =
#include "gbuffer.vert.glsl"
;

const char* mainFragSource =
#include "main.frag.glsl"
;

const char* mainVertSource =
#include "main.vert.glsl"
;