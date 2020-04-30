#version 330 core

uniform sampler2D diffuse_texture;
uniform sampler2D displacement_texture;
uniform sampler2D framebuffer_texture;
uniform mat3 fragcoord2uv;
uniform float backbuffer;
uniform float game_time;
uniform vec2 animate;
uniform vec2 displacement_animate;

in vec4 diffuse_var;
in vec2 texcoord_var;

out vec4 fragColor;


// MIT
// https://github.com/tobspr/GLSL-Color-Spaces/blob/master/ColorSpaces.inc.glsl
const float SRGB_GAMMA = 1.0 / 2.2;
// Converts a linear rgb color to a srgb color (approximated, but fast)
vec3 rgb_to_srgb_approx(vec3 rgb) {
    return pow(rgb, vec3(SRGB_GAMMA));
}

void main(void)
{
  if (backbuffer == 0.0)
  {
    vec4 color = texture(diffuse_texture, texcoord_var.st + (animate * game_time));
    color.rgb = rgb_to_srgb_approx(color.rgb);
    color *= diffuse_var;
    fragColor = color;
  }
  else if (true)
  {
    vec4 pixel = texture(displacement_texture, texcoord_var.st + (displacement_animate * game_time));
    vec2 displacement = (pixel.rg - vec2(0.5, 0.5)) * 255;
    float alpha = pixel.a;

    vec2 uv = (fragcoord2uv * (gl_FragCoord.xyw + vec3(displacement.xy * alpha, 0))).xy;
    uv = vec2(uv.x, 1.0 - uv.y);
    vec4 back_color = texture(framebuffer_texture, uv);

    vec4 color =  texture(diffuse_texture, texcoord_var.st + (animate * game_time));
    color.rgb = rgb_to_srgb_approx(color.rgb);
    color *= diffuse_var;
    fragColor = vec4(mix(color.rgb, back_color.rgb, alpha), color.a);
  }
  else
  {
    // water reflection
    vec4 color =  diffuse_var * texture(diffuse_texture, texcoord_var.st);
    vec2 uv = (fragcoord2uv * gl_FragCoord.xyw).xy + vec2(0, 0.05);
    uv.x = uv.x + 0.005 * sin(game_time + uv.y * 100);
    uv = vec2(uv.x, 1.0 - uv.y);
    vec4 back_color = texture(framebuffer_texture, uv);
    if (backbuffer == 0.0)
      fragColor = color;
    else
      if (uv.y > 0.5)
        fragColor = vec4(mix(vec3(0,0,0.75), mix(color.rgb, back_color.rgb, 0.95 * backbuffer), (1.2 - uv.y) * (1.2 - uv.y)), 1.0);
      else
        fragColor = color;
  }
}

/* EOF */
