#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

uniform vec3 cameraPosition;// = vec3(0.0, 0.0, 0.0);
uniform vec3 directionalLight = vec3(1.0, -1.0, -1.0);
//uniform vec3 directionalLight = vec3(0, 0.0, -1.0);

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
	vec3 normal;
};

struct Object
{
	int type; // 0: sphere, 1: plane
	vec3 center;
	vec3 color;
	float radius; // sphere
	vec3 normal; // plane
};

struct Sphere
{
	vec3 center;
	float radius;
	vec3 color;
};

Object scene[2];
//uniform Object pl = { 0, vec3(0.0, -2.0, -5.0), 2.0, vec3(0), vec3(0.0, 1.0, 0.0) };
//uniform Object pl = {1, vec3(0.0, -1.0, 0.0), 0.0, vec3(0.0, -1.0, 0.0), vec3(0.5, 0.5, 0.5) };
//uniform Sphere sph = { vec3(0.0, 0.0, -5.0), 2.0, vec3(0.0, 1.0, 0.0) };

//uniform Object plane;

// out keyword gives pass by reference (without initialization) (kind of?)
// intersection test
void intersect(const in Ray ray, inout Hit hit, const in Object obj)
{
	if (obj.type == 0)
	{
		float a = dot(ray.dir, ray.dir);
		float b = dot(2 * ray.dir, (ray.pos - obj.center));
		float c = dot(ray.pos - obj.center, ray.pos - obj.center) - (obj.radius*obj.radius);

		if (sqrt(b*b - 4*a*c) >= 0) 
		{
			float t1 = (-b - sqrt(b*b - 4*a*c)) / 2*a;
			float t2 = (-b + sqrt(b*b - 4*a*c)) / 2*a;
			float t = min(t1, t2);
						
			hit.t = t;
			hit.pos = ray.pos + t*ray.dir;
			hit.normal = normalize(hit.pos - obj.center);
			hit.flag = true;
		}
	}
	else if (obj.type == 1)
	{
		// get ray and plane parallel?
		// then check if dot product of ray and normal == 0 (or maybe >0)
		float denom = dot(obj.normal, ray.dir);
		//if (denom == 0) return;

		float t = dot(obj.center - ray.pos, obj.normal) / denom;
		if (t >= 0)
		{
			hit.t = t;
			hit.pos = ray.pos + t*ray.dir;
			hit.normal = obj.normal;
			hit.flag = true;
			//return true;
		}

	}

	// unknown object type
	//return false;
}

vec3 trace(const in Ray ray)
{	
	float t = 99999;
	Hit hit;

	for (int i = 0; i < scene.length; i++)
	{
		hit.flag = false;
		hit.t = 99999;
		intersect(ray, hit, scene[i]);
		if (hit.flag && hit.t < t)
		{
			t = hit.t;
			// adding color
			float ndotl = clamp(dot(hit.normal, normalize(-directionalLight)), 0.05, 1.0);

			return /*hit.normal*/scene[i].color * ndotl;
		}
	}

	return vec3(0, 0, 0);
}


void main()
{
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

	// image/screen/texture size
	// in pixels
	vec2 size = imageSize(screen);

	// calculate x and y components of view ray from pixel co-ord
	float x = (float(pixelCoord.x * 2 - size.x) / size.x);
	float y = (float(pixelCoord.y * 2 - size.y) / size.y);

	Ray viewRay;
	// add camera position to move near plane when camera moves
	viewRay.pos = vec3(x, y, -1.0) + cameraPosition;
	// don't forget to normalize your view direction vector!
	viewRay.dir = normalize(viewRay.pos - cameraPosition);

	// populate scene
	Object sph = { 0, vec3(0.0, 0.0, -5.0), vec3(0.0, 1.0, 0.0), 2.0, vec3(0)};
	scene[0] = sph;
	Object pl = { 1, vec3(0.0, -2.0, 0.0), vec3(0.5, 0.5, 0.5), 0.0, vec3(0.0, 1.0, 0.0)};
	scene[1] = pl;

	col = vec4(trace(viewRay), 1.0);
	//col = vec4(abs(x), abs(y), 0.0, 1.0);

	imageStore(screen, pixelCoord, col);
}
