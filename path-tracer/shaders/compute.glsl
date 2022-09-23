#version 460 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D screen;

uniform vec3 cameraPosition;// = vec3(0.0, 0.0, 0.0);
vec3 directionalLight = vec3(1.0, -1.0, -1.0);
//uniform vec3 directionalLight = vec3(0, 0.0, -1.0);

uniform mat4 viewMatrix;

//uniform int time;

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

// PRNG https://stackoverflow.com/a/4275343/18375905
float rand(vec2 co)
{
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

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
	closestHit.flag = false;
	closestHit.color = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < scene.length; i++)
	{
		hit.flag = false;
		intersect(ray, hit, scene[i]);

		if (hit.flag && hit.t < t)
		{
			t = hit.t;
			closestHit = hit;
			//if (inShadow(hit.pos)) return hit.color*0.1;
		}
	}

	hit = closestHit;
	float ndotl = clamp(dot(closestHit.normal, normalize(-directionalLight)), 0.05, 1.0);
	return closestHit.color * ndotl;
}


void main()
{
	vec3 col = vec3(0.0, 0.0, 0.0);
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

	// image/screen/texture size
	// in pixels
	vec2 size = imageSize(screen);

	vec2 pixelSize;
	// you do have permission to shoot me now
	pixelSize.x = float(((size.x/2)+1) * 2 - size.x) / size.x;
	pixelSize.y = float(((size.y/2)+1) * 2 - size.y) / size.y;

	// calculate x and y components of view ray from pixel co-ord
	float x = (float(pixelCoord.x * 2 - size.x) / size.x);
	float y = (float(pixelCoord.y * 2 - size.y) / size.y);

	// populate scene
	Object sph = { 0, vec3(-2.5, 0.0, -7.0), vec3(0.0, 1.0, 0.0), 2.0, vec3(0)};
	scene[0] = sph;
	Object sph2 = { 0, vec3(2.5, 0.0, -7.0), vec3(1.0, 0.0, 0.0), 2.0, vec3(0)};
	scene[1] = sph2;
	//Object sph3 = { 0, vec3(0.0, 0.0, 0.0), vec3(150.0/255.0, 220.0/255.0, 240.0/255.0), 498.0, vec3(0)};
	//scene[2] = sph3;
	Object pl = { 1, vec3(0.0, -2.0, 0.0), vec3(0.9, 0.9, 0.9), 0.0, vec3(0.0, 1.0, 0.0)};
	scene[2] = pl;

	Ray ray;
	vec4 temp = (vec4(x, y, -1.0, 1.0)*viewMatrix) + vec4(cameraPosition, 0.0);
	ray.pos = vec3(temp.x, temp.y, temp.z);// + cameraPosition;
	ray.dir = normalize(ray.pos - cameraPosition);
	ray.pos = cameraPosition;

	// trace to a defined depth here
	int passes = 4;
	int depth = 2;
	int samples = 1;
	//for (int p = 0; p < passes; p++)
	//{
	for (int d = 0; d < depth; d++)
	{	
		Hit hit;

		// Comment this all out and figure out something better.
		// Or, just no anti-aliasing.
		// try center first so we can break if nothing is there
		ray.pos.x += pixelSize.x / 2;
		ray.pos.y += pixelSize.y / 2;
		// yeah, i know, but what if this works?
		
		/*
		ray.pos.x -= pixelSize.x / 2;
		ray.pos.y -= pixelSize.y / 2;
		ray.pos.x += pixelSize.x;
		col += vec4(trace(ray, hit), 1.0);
		ray.pos.x -= pixelSize.x;
		ray.pos.y += pixelSize.y;
		col += vec4(trace(ray, hit), 1.0);
		ray.pos.x += pixelSize.x;
		col += vec4(trace(ray, hit), 1.0);
		*/
		/*
		int divby = 1;
		for (int s = 0; s < samples; s++)
		{
			
			if (d == 1)
			{
				col += trace(ray, hit)/2;
			}
			else
			{
				col += trace(ray, hit)/2;
			}
		
			if (!hit.flag) break;
			divby++;
		}
		*/
		

		

		col += trace(ray, hit);

		if (!hit.flag) break;
		
		col /= 3;

		//col += vec4(newCol, 0.0);

		// next ray
		/*
		vec2 co = vec2(time*x, time*y);
		vec2 co1 = vec2(time*y*2, time*x*2);
		vec2 co2 = vec2(time*ray.dir.y*3, time*ray.dir.x*3);
		*/

		vec2 co = vec2(x, y);
		vec2 co1 = vec2(y*2, x*2);
		vec2 co2 = vec2(ray.dir.y*3, ray.dir.x*3);

		ray.dir = normalize(vec3(rand(co), rand(co1), rand(co2)));
		if (dot(hit.normal, ray.dir) < 0.0) ray.dir = -ray.dir;

		ray.pos = hit.pos + 0.01*ray.dir;
		

	}
	//}

	//col /= passes;
	
	//col /= 2.0;

	//col = sqrt(col);
	
	imageStore(screen, pixelCoord, vec4(col, 1.0));
}