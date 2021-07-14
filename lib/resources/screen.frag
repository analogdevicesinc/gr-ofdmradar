#version 400

uniform int width;
uniform int height;
uniform float minV;
uniform float maxV;
uniform float rangeV;

uniform sampler2D screenData;

out vec4 color;

/////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2019 Google LLC.
// SPDX-License-Identifier: Apache-2.0

// Polynomial approximation in GLSL for the Turbo colormap
// Original LUT: https://gist.github.com/mikhailov-work/ee72ba4191942acecc03fe6da94fc73f

// Authors:
//   Colormap Design: Anton Mikhailov (mikhailov@google.com)
//   GLSL Approximation: Ruofei Du (ruofei@google.com)
vec3 TurboColormap(in float x) {
  const vec4 kRedVec4 = vec4(0.13572138, 4.61539260, -42.66032258, 132.13108234);
  const vec4 kGreenVec4 = vec4(0.09140261, 2.19418839, 4.84296658, -14.18503333);
  const vec4 kBlueVec4 = vec4(0.10667330, 12.64194608, -60.58204836, 110.36276771);
  const vec2 kRedVec2 = vec2(-152.94239396, 59.28637943);
  const vec2 kGreenVec2 = vec2(4.27729857, 2.82956604);
  const vec2 kBlueVec2 = vec2(-89.90310912, 27.34824973);
  
  x = clamp(x, 0, 1);
  vec4 v4 = vec4( 1.0, x, x * x, x * x * x);
  vec2 v2 = v4.zw * v4.z;
  return vec3(
    dot(v4, kRedVec4)   + dot(v2, kRedVec2),
    dot(v4, kGreenVec4) + dot(v2, kGreenVec2),
    dot(v4, kBlueVec4)  + dot(v2, kBlueVec2)
  );
}
/////////////////////////////////////////////////////////////////////////////////////////

float fftshift(in float x) {
    if (x > 0.5)
        return x - 0.5;
    else
        return x + 0.5;
}

void main()
{
    vec2 texCoords = vec2(clamp(gl_FragCoord.y / height * rangeV, 0, 1),
            fftshift(gl_FragCoord.x / width));

    vec2 texColor = texture(screenData, texCoords).rg;
    float intensity = sqrt(texColor.r * texColor.r + texColor.g * texColor.g);
    intensity = clamp((intensity - minV) / (maxV - minV), 0, 1);
    color = vec4(TurboColormap(intensity), 0);
}
