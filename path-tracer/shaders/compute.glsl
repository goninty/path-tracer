#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

uniform vec3 cameraPosition;// = vec3(0.0, 0.0, 0.0);
uniform vec3 directionalLight = vec3(1.0, -1.0, 0.0);
//uniform vec3 directionalLight = vec3(0, 0.0, -1.0);

uniform mat4 viewMatrix;

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
	vec3 color; // figure out a better way to do this?
	// ie try to store a hit.what, if possible
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

Object scene[3];

// out keyword gives pass by reference (without initialization) (kind of?)
// intersection test
void intersect(const in Ray ray, inout Hit hit, inout Object obj)
{
	if (obj.type == 0)
	{
		float a = dot(ray.dir, ray.dir);
		float b = 2 * dot(ray.dir, (ray.pos - obj.center));
		float c = dot(ray.pos - obj.center, ray.pos - obj.center) - (obj.radius*obj.radius);
		
		if (sqrt(b*b - 4*a*c) >= 0.0) 
		{
			float t1 = (-b - sqrt(b*b - 4*a*c)) / 2*a;
			float t2 = (-b + sqrt(b*b - 4*a*c)) / 2*a;

			if (t2 < 0) return;
			float t = t1;

			// the below if statement was dredged from my CM30075 coursework
			// and fixed a bug I had been chasing for hours...
			if (t > 0)
			{
				hit.t = t; 
				hit.pos = ray.pos + t*ray.dir;
				hit.normal = normalize(hit.pos - obj.center);
				hit.color = obj.color;
				hit.flag = true;
			}
		}
	}
	else if (obj.type == 1)
	{
		// get ray and plane parallel?
		// then check if dot product of ray and normal == 0 (or maybe >0)
		float denom = dot(obj.normal, ray.dir);

		float t = dot(obj.center - ray.pos, obj.normal) / denom;
		if (t > 0)
		{
			hit.t = t;
			hit.pos = ray.pos + t*ray.dir;
			hit.normal = obj.normal;
			hit.color = obj.color;
			hit.flag = true;
		}

	}
}

// is this point in shadow?
bool inShadow(vec3 pos)
{
	Ray shRay;
	shRay.dir = -directionalLight;
	shRay.pos = pos + 0.001*shRay.dir;

	Hit shHit;

	for (int i = 0; i < scene.length; i++)
	{
		shHit.flag = false;
		intersect(shRay, shHit, scene[i]);
		if (shHit.flag) return true;
	}

	return false;
}

vec3 trace(const in Ray ray, inout Hit hit)
{	
	float t = 99999; // closest
	Hit closestHit;
	closestHit.color = vec3(0, 0, 0);

	for (int i = 0; i < scene.length; i++)
	{
		hit.flag = false;
		intersect(ray, hit, scene[i]);

		if (hit.flag && hit.t < t)
		{
			t = hit.t;
			closestHit = hit;
			if (inShadow(hit.pos)) return hit.color*0.1;
		}
	}

	float ndotl = clamp(dot(closestHit.normal, normalize(-directionalLight)), 0.05, 1.0);
	return closestHit.color * ndotl;
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

	// populate scene
	Object sph = { 0, vec3(0.0, 0.0, -5.0), vec3(0.0, 1.0, 0.0), 2.0, vec3(0)};
	scene[0] = sph;
	Object sph2 = { 0, vec3(0.0, 0.0, 5.0), vec3(1.0, 0.0, 0.0), 2.0, vec3(0)};
	scene[1] = sph2;
	Object pl = { 1, vec3(0.0, -2.0, 0.0), vec3(0.5, 0.5, 0.5), 0.0, vec3(0.0, 1.0, 0.0)};
	scene[2] = pl;

	Ray viewRay;
	vec4 temp = (vec4(x, y, -1.0, 1.0)*viewMatrix) + vec4(cameraPosition, 0.0);
	viewRay.pos = vec3(temp.x, temp.y, temp.z);// + cameraPosition;
	viewRay.dir = normalize(viewRay.pos - cameraPosition);
	viewRay.pos = cameraPosition;

	Hit viewHit;
	col = vec4(trace(viewRay, viewHit), 1.0);

	imageStore(screen, pixelCoord, col);
}
