#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

uniform vec3 cameraPosition = vec3(0.0, 0.0, 0.0);
uniform vec3 directionalLight = vec3(1.0, -1.0, -1.0);

struct Ray
{
	vec3 pos;
	vec3 dir;
};

struct Hit
{
	bool flag;
	float t;
	vec3 pos;
};

// Create enum to hold type of objects? Instead of strings.

struct Object
{
	// GLSL does not have strings.
	// 0 = sphere, 1 = plane.
	int type;
	vec3 center;

	// Sphere
	float radius;

	// Plane
	vec3 normal;

	vec3 color;
};

struct Sphere
{
	vec3 center;
	float radius;
	vec3 color;
};


//uniform Object pl = { 0, vec3(0.0, -2.0, -5.0), 2.0, vec3(0), vec3(0.0, 1.0, 0.0) };
uniform Object pl = {1, vec3(0.0, -1.0, 0.0), 0.0, vec3(0.0, -1.0, 0.0), vec3(0.5, 0.5, 0.5) };
//uniform Sphere sph = { vec3(0.0, 0.0, -5.0), 2.0, vec3(0.0, 1.0, 0.0) };

//uniform Object plane;

// out keyword gives pass by reference (without initialization) (kind of?)
// intersection test
bool intersect(const in Ray ray, out Hit hit, const in Object obj)
{
	if (obj.type == 0)
	{
		const float a = dot(ray.dir, ray.dir);
		const float b = dot(2 * ray.dir, (ray.pos - obj.center));
		const float c = dot(ray.pos - obj.center, ray.pos - obj.center) - (obj.radius*obj.radius);

		if (sqrt(b*b - 4*a*c) >= 0) 
		{
			float t1 = (-b - sqrt(b*b - 4*a*c)) / 2*a;
			float t2 = (-b + sqrt(b*b - 4*a*c)) / 2*a;
			float t = min(t1, t2);
		
			hit.t = t;
			hit.pos = ray.pos + t*ray.dir;
			hit.flag = true;
			return true;
		}
	}
	else if (obj.type == 1)
	{
		// get ray and plane parallel?
		// then check if dot product of ray and normal == 0 (or maybe >0)
		float denom = dot(pl.normal, ray.dir);
		if (denom == 0) return false;

		float t = dot(pl.center - ray.pos, pl.normal)  / dot(pl.normal, ray.dir);
		if ( t >= 0)
		{
			return true;
		}

	}

	// unknown object type
	return false;
}

vec3 trace(const in Ray viewRay)
{	
	Hit hh;
	
	if (intersect(viewRay, hh, pl))
	{
		// adding color
		vec3 normal = normalize(hh.pos - pl.center);
		float ndotl = clamp(dot(normal, normalize(-directionalLight)), 0.05, 1.0);
		return pl.color * ndotl;
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
	float x = (float(pixelCoord.x * 2 - size.x) / size.x);
	float y = (float(pixelCoord.y * 2 - size.y) / size.y);

	Ray viewRay;
	viewRay.pos = vec3(x, y, -1.0);
	// don't forget to normalize your view direction vector!
	viewRay.dir = normalize(viewRay.pos - cameraPosition);

	col = vec4(trace(viewRay), 1.0);

	imageStore(screen, pixelCoord, col);
}
