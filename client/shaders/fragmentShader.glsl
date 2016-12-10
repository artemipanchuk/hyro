precision mediump float;

#define EPS       0.001
#define EPS1      0.01
#define PI        3.1415926535897932
#define HALFPI    1.5707963267948966
#define QUARTPI   0.7853981633974483
#define ROOTTHREE 0.57735027
#define HUGEVAL   1e20

#define KA 0.1
#define KD 0.9

#define FX_DIFFUSE
#define FX_OCCLUSION
#define FX_SHADOW
#define FX_REFLECTION
#define MAX_STEPS 64

varying vec2 vUv;

uniform vec3  uCamCenter;
uniform vec3  uCamPos;
uniform vec3  uCamUp;
uniform float uAspect;
uniform vec3  uLightP;
uniform vec3  uLightC;
uniform vec3 uObjectsP[25];
uniform vec3 uObjectsC[25];
uniform float uObjectsRC[25];
uniform float uObjectsOC[25];

float gMin = 0.0;
float gMax = HUGEVAL;

vec3 currentColor       = vec3(0.3, 0.5, 1.0);
vec3 currentLogicColor  = vec3(0.3, 0.5, 1.0);

int currentId = -1;

bool  ignoreRC  = false;
float currentRC = 0.0;

vec3  currentTransparentColor;
bool  currentTransparent = false;
bool  ignoreOC  = false;
float currentOC = 1.0;

bool  currentHit = false;
vec3  currentPosition, currentNormal;

float maxcomp(in vec3 p) {
	return max(p.x,max(p.y,p.z));
}

// Primitive Objects

float Sph(vec3 p, float s) {
	return length(p) - s;
}

float Box(vec3 p, vec3 b) {
	vec3  di = abs(p) - b;
	float mc = maxcomp(di);
	return min(mc,length(max(di, 0.0)));
}

float Cyl(vec3 p, vec3 c) {
	return length(p.xz-c.xy)-c.z;
}

float Tor(vec3 p, vec2 t) {
	vec2 q = vec2(length(p.xz)-t.x,p.y);
	return length(q)-t.y;
}

// Operations

float Uni(float d1, float d2) {
	return min(d1, d2);
}

float Sub(float d1, float d2) {
	return max(-d1,d2);
}

float Int(float d1, float d2) {
	return max(d1,d2);
}

float getDist(in vec3 p) {
	float x    = HUGEVAL;
	float oldX = HUGEVAL;
	vec3 s     = p;

#OBJECTS

	return x;
}

vec3 getNormal(in vec3 pos) {
	vec3 eps = vec3(EPS, 0.0, 0.0);
	vec3 nor;

	nor.x = getDist(pos+eps.xyy) - getDist(pos-eps.xyy);
	nor.y = getDist(pos+eps.yxy) - getDist(pos-eps.yxy);
	nor.z = getDist(pos+eps.yyx) - getDist(pos-eps.yyx);

	return normalize(nor);
}

float intersectDist(in vec3 ro, in vec3 rd) {
	float t    = gMin;
	float dist = -1.0;

	for (int i = 0; i < MAX_STEPS; ++i) {
		float dt = getDist(ro + rd*t);

		if (dt < EPS) {
			dist = t;
			break;
		}

		t += dt;

		if (t > gMax)
			break;
	}

	return dist;
}

#define SS_K 50.0
float getShadow(in vec3 pos, in vec3 toLight) {
	float shadow = 1.0;
	float lightDist = distance(-uLightP, pos);

	float t = EPS1;
	float dt;

	for(int i = 0; i < MAX_STEPS; ++i) {
		dt = getDist(pos+(toLight*t));

		if (dt < EPS)
			return 0.0;

		shadow = min(shadow, SS_K*(dt/t));

		t += dt;

		if (t > lightDist)
			break;
	}

	return clamp(shadow, 0.0, 1.0);
}

#define AO_K      1.5
#define AO_DELTA  0.2
#define AO_N      5
float getAO(in vec3 pos, in vec3 nor) {
	float sum = 0.0;
	float weight = 0.5;
	float delta = AO_DELTA;

	for (int i=0; i<AO_N; ++i) {
		sum += weight * (delta - getDist(pos+nor*delta));

		delta += AO_DELTA;
		weight *= 0.5;
	}

	return clamp(1.0 - AO_K*sum, 0.0, 1.0);
}

vec3 rayMarch(in vec3 ro, in vec3 rd) {
	float t = intersectDist(ro, rd);

	if (t > 0.0) {
		vec3 pos = ro + rd*t;
		vec3 nor = getNormal(pos-rd*EPS);

		vec3 color = currentColor;

		#ifdef FX_DIFFUSE

		vec3 toLight = normalize(uLightP - pos);

		float shadow = 1.0;

		#ifdef FX_SHADOW

		ignoreRC = true;
		ignoreOC = true;
		shadow = getShadow(pos, toLight);
		ignoreRC = false;
		ignoreOC = false;

		#endif

		color *= (KA + KD*max(dot(toLight, nor), 0.0)*shadow);

		color.x += color.x*uLightC.x;
		color.y += color.y*uLightC.y;
		color.z += color.z*uLightC.z;

		#endif

		#ifdef FX_OCCLUSION

		ignoreRC = true;
		ignoreOC = true;
		float aoc = getAO(pos, nor);
		ignoreOC = false;
		ignoreRC = false;

		color *= aoc;

		#endif

		currentHit = true;
		currentPosition = pos;
		currentNormal = nor;
		currentColor = color;

		return color;
	}

	currentHit = false;

	return vec3(0.0);
}

vec3 render(in vec3 ro, in vec3 rd) {
	currentRC = 0.0;
	currentOC = 1.0;

	vec3 color        = rayMarch(ro, rd);
	vec3 overlayColor = vec3(0.0);

	#ifdef FX_REFLECTION

	vec3 reflRay;
	bool reflRayDefined = false;

	for (int i = 0; i < 5; ++i) {
		if (currentRC == 0.0 && currentOC == 1.0)
			break;

		if (currentHit && currentOC < 1.0 && currentRC > 0.0) {
			float oldRC   = currentRC;
			float oldOC   = currentOC;
			vec3 oldColor = currentColor;

			reflRay = reflect(rd, currentNormal);
			reflRayDefined = true;

			currentTransparentColor = currentLogicColor;

			color = overlayColor
				+ color*(1.0 - oldRC)*oldOC
				+ rayMarch(currentPosition, rd)*(1.0 - oldOC) * currentOC * (1.0 - currentRC)
				+ rayMarch(currentPosition+reflRay, reflRay*1.01) * oldRC * currentOC * (1.0 - currentRC);

			currentTransparentColor = vec3(-1.0);

			overlayColor += oldColor*oldOC;
		} else if (currentHit && currentRC > 0.0) {
			float oldRC = currentRC;
			vec3 oldColor = currentColor;
			reflRayDefined = true;

			reflRay = reflect(rd, currentNormal);

			color = overlayColor + color*(1.0 - oldRC) + rayMarch(currentPosition+reflRay, reflRay) * oldRC * currentOC * (1.0-currentRC);

			overlayColor += oldColor*(1.0 - oldRC);
		} else if (currentHit && currentOC < 1.0) {
			float oldOC   = currentOC;
			vec3 oldColor = currentColor;

			currentTransparentColor = currentLogicColor;
			color = overlayColor + color*oldOC + rayMarch(currentPosition, (reflRayDefined ? reflRay : rd)*1.01)*(1.0 - oldOC)*currentOC*(1.0-currentRC);
			currentTransparentColor = vec3(-1.0);

			overlayColor += oldColor*oldOC;
		}
	}

	#endif

	return color;
}

void main(void) {
	vec3 C = normalize(uCamCenter-uCamPos);
	vec3 A = uAspect*normalize(cross(C,uCamUp));
	vec3 B = -normalize(uCamUp);

	vec3 rd = normalize(C + (2.0*vUv.x-1.0)*ROOTTHREE*A + (2.0*vUv.y-1.0)*ROOTTHREE*B);
	vec3 ro = uCamPos;

	vec3 color = render(ro, rd);

	if (color.x > 1.0)
		color.x = 1.0;

	if (color.y > 1.0)
		color.y = 1.0;

	if (color.z > 1.0)
		color.z = 1.0;

	gl_FragColor.rgb = color;
	gl_FragColor.a = 1.0;
}