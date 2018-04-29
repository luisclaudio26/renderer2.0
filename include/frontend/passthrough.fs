#version 450

// Texture sampler: this is our actual rendered
// image and should be the same size as the screen
//uniform sampler2D img;
uniform vec4 color;

in vec2 uv_frag;
out vec4 pixel;

void main()
{
  //pixel = img.sample(uv_frag);
  pixel = color;
}
