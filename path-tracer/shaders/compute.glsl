#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

uniform vec3 cameraPosition = vec3(0.0, 0.0, 0.0);
uniform vec3 directionalLight = vec3(0.0, -1.0, 0.0);

struct Ray
{
	vec3 pos;
	vec3 dir;
};

struct Sphere
{
	vec3 center;
	float radius;
	vec3 color;
};

uniform Sphere sph = { vec3(0.0, 0.0, -5.0), 2.0, vec3(0.0, 0.0, 0.5) };



vec3 trace(struct Ray viewRay)
{
	
	float a = dot(viewRay.dir, viewRay.dir);
	float b = dot(2 * viewRay.dir, (viewRay.pos - sph.center));
	float c = dot(viewRay.pos - sph.center, viewRay.pos - sph.center) - (sph.radius*sph.radius);

	// check discriminant first
	// if discriminant is positive, there's intersection
	if (sqrt(b*b - 4*a*c) >= 0.0)
	{
		float t = (-b + sqrt(b*b - 4*a*c)) / 2*a;

		// calculate hit point
		vec3 hitPos = normalize(viewRay.pos + t);
		//return hitPos;
		// calclulate normal at hit point
		vec3 normal = hitPos - sph.center;
		//return normal;

		//return hitPos;

		float col = dot(normal, -normalize(directionalLight));
		
		return sph.color * col;
		//return vec3(col, col, col);
	}

	return vec3(0, 0, 0);
}


void main()
{
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

	// image/screen/texture size
	vec2 size = imageSize(screen);

	// calculate x and y components of view ray from pixel co-ord
	float x = -(float(pixelCoord.x * 2 - size.x) / size.x);
	float y = -(float(pixelCoord.y * 2 - size.y) / size.y);

	struct Ray viewRay;
	viewRay.pos = vec3(x, y, -1.0);
	viewRay.dir = viewRay.pos - cameraPosition;

	col = vec4(trace(viewRay), 1.0);

	imageStore(screen, pixelCoord, col);
}
