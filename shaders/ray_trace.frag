#version 450 core

out vec4 FragColor;

uniform uint time;
uniform uint frameCount;
uniform uvec2 windowSize;
uniform vec2 fov;
uniform vec2 aperture;
uniform vec3 cameraPos;
uniform vec3 cameraUp;
uniform vec3 cameraDir;
layout(binding=1) uniform sampler2D tex;

uint randState;

struct Face {
	vec4 v0;
	vec4 v1;
	vec4 v2;
	vec4 normal;
	vec4 color;
	uint flags;
};

struct Ray {
	vec3 origin;
	vec3 direction;
	vec3 color;
	float dist;
};


struct Intersection {
	bool hit;
	Ray newRay;
};

layout(std430, binding = 0) buffer objSSBO
{
    Face[] objFaces;
};

uint PCGHash()
{
	randState = randState * 747796405u + 2891336453u;
	uint word = ((randState >> ((randState >> 28u) + 4u)) ^ randState) * 277803737u;
	return (word >> 22u) ^ word;
}

Ray getRay()
{
	vec2 point = gl_FragCoord.xy / windowSize - vec2(0.5);
	vec3 cameraRight = normalize(cross(cameraDir, cameraUp));
	vec3 rayOrigin = cameraRight * aperture.x * point.x + cameraUp * aperture.y * point.y + cameraPos;	// Frame aperture by camera orientation vectors and translate to camera position

	vec2 dirDegrees = point * fov;
	vec3 rayDir = cameraRight * tan(radians(dirDegrees.x)) + cameraUp * tan(radians(dirDegrees.y)) + cameraDir;
	return Ray(rayOrigin, normalize(rayDir), vec3(1.0), 0.0);
}

Intersection doesIntersect(Ray ray, Face face) {
	vec3 normal = face.normal.xyz;
	vec3 v0 = face.v0.xyz;
	vec3 v1 = face.v1.xyz;
	vec3 v2 = face.v2.xyz;
	Intersection result = {false, {vec3(0.0), vec3(0.0), vec3(0.0), 0.0} };

	float denominator = dot(ray.direction, normal);
	if (denominator > -0.0001) {
		return result;
	}

	// "time" of intersection along ray
	float t = dot(v0 - ray.origin, normal) / denominator;
	if (t < 0) {
		return result;
	}

	// Intersection point
	vec3 p = ray.origin + t * ray.direction;

	// Check side 1
	vec3 s1 = v1 - v0;
	vec3 p1 = p - v0;
	if (dot(cross(s1, p1), normal) < 0) {
		return result;
	}

	// Check side 2
	vec3 s2 = v2 - v1;
	vec3 p2 = p - v1;
	if (dot(cross(s2, p2), normal) < 0) {
		return result;
	}

	// Check side 3
	vec3 s3 = v0 - v2;
	vec3 p3 = p - v2;
	if (dot(cross(s3, p3), normal) < 0) {
		return result;
	}

	result.hit = true;
	result.newRay.origin = p;
	result.newRay.dist = ray.dist + t;
	result.newRay.color = ray.color * face.color.xyz;

	// Randomly select between a diffuse reflection and a specular reflection
	if (PCGHash() / float(0xFFFFFFFFU) > 0.01) {	
		result.newRay.direction = normalize(vec3(PCGHash(), PCGHash(), PCGHash()) / float(0xFFFFFFFFU) * 2.0 - 1.0);
		if (dot(result.newRay.direction, normal) < 0) {
			result.newRay.direction = result.newRay.direction * -1.0;
		}
	} else {
		result.newRay.direction = ray.direction - 2.0 * dot(ray.direction, normal) * normal;
	}

	return result;
}

void main()
{	
	// Initialize random state based on pixel coordinates
	randState = uint(gl_FragCoord.x*windowSize.y + gl_FragCoord.y) + time*windowSize.x*windowSize.y;

	// Parameters
	const int numRays = 32;
	const int numBounces = 4;
	const vec3 ambient = vec3(0.05, 0.05, 0.05);
	vec3 color = vec3(0.0, 0.0, 0.0);

	for (int rayI = 0; rayI < numRays; rayI++) {
		Ray ray = getRay();
		vec3 rayColor = vec3(0.0, 0.0, 0.0);

		// Bounces until a light is hit or max bounces reached
		for (int bounce = 0; bounce < numBounces; bounce++) {
			float depth = uintBitsToFloat(0x7F800000);
			Ray newRay = ray;
			bool hit = false;
			bool light = false;
			vec3 lightColor = vec3(0.0, 0.0, 0.0);
			for (int i = 0; i < objFaces.length(); i++) {
				Intersection intersect = doesIntersect(ray, objFaces[i]);
				float d = length(intersect.newRay.origin - ray.origin);
				if (intersect.hit && length(intersect.newRay.origin - ray.origin) < depth) {
					hit = true;
					newRay = intersect.newRay;
					// Any hit should give some ambient illumination
					rayColor = ambient;
					if ((objFaces[i].flags & 0x1u) != 0x0u) {
						light = true;
						lightColor = objFaces[i].color.xyz;
					} else {
						light = false;
					}
					depth = d;
				}
			}
			if (!hit) {
				// If no intersection occurred checking more "bounces" is pointless
				break;
			}

			if (light) {
				rayColor = lightColor;
				break;
			}

			ray = newRay;
		}

		color += rayColor / float(numRays);
	}

	//FragColor = vec4((frameCount * texture(tex, gl_FragCoord.xy/windowSize).xyz + color) / (frameCount + 1), 1.0);
	FragColor = vec4((0.8 * texture(tex, gl_FragCoord.xy/windowSize).xyz + 0.2*color), 1.0);
}