# Path Tracer

I wanted to write a path tracer. Initially I wanted to write it in C++ and somehow display it using OpenGL. Then I realised I should probably write it as a fragment shader. Then I later realised I should probably write it as a compute shader instead. This is that.

All of the path tracing logic is written in GLSL. This is a bit bothersome, and quite limiting. However, it means calculations are made on the GPU in parallel for each pixel.

OpenGL is used to render a quad and an empty texture is bound which the compute shader writes to, displaying the image.

## Useful Resources
[OpenGL Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2) by Yan Chernikov
[Learn OpenGL](https://learnopengl.com/) by Joey de Vries (and Jonas Sorgenfrei on Compute Shaders)
[Modern OpenGL - Compute Shaders](https://www.youtube.com/watch?v=nF4X9BIUzx0) by Victor Gordan
[Scratchapixel](https://www.scratchapixel.com/) (My absolute favourite resource on all things ray tracing)
[Realistic Ray Tracing](https://www.amazon.co.uk/Realistic-Ray-Tracing-Peter-Shirley/dp/1568811101) by Peter Shirley
[Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) by Peter Shirley
My CM30075 lecturer Ken ;) (While I was still a student)

## TODO
- ~~Ray trace.~~
- Phong lighting.
- Reflection / refraction.
- Moveable camera.
- Read in mesh files (.obj).
- Actually path trace.
- Code cleanup.