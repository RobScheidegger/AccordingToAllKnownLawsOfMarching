#include "fractal.h"

std::optional<Intersect> Fractal::intersect(Ray ray) const {
    // Should never actual be used: Fractals only support raymarching
    throw std::runtime_error("Error: Fractals only support raymarching.");
}

float Fractal::mandelbulbSDF(glm::vec4 p) const {
    glm::vec3 w = p;
    float m = dot(w,w);

    glm::vec4 trap = glm::vec4(abs(w),m);
    float dz = 1.0;

    for( int i=0; i<20; i++ )
    {
        // polynomial version (no trigonometrics, but MUCH slower)
        float m2 = m*m;
        float m4 = m2*m2;
        dz = 8.0*sqrt(m4*m2*m)*dz + 1.0;

        float x = w.x; float x2 = x*x; float x4 = x2*x2;
        float y = w.y; float y2 = y*y; float y4 = y2*y2;
        float z = w.z; float z2 = z*z; float z4 = z2*z2;

        float k3 = x2 + z2;
        float k2 = glm::inversesqrt( k3*k3*k3*k3*k3*k3*k3 );
        float k1 = x4 + y4 + z4 - 6.0*y2*z2 - 6.0*x2*y2 + 2.0*z2*x2;
        float k4 = x2 - y2 + z2;

        w.x = p.x +  64.0*x*y*z*(x2-z2)*k4*(x4-6.0*x2*z2+z4)*k1*k2;
        w.y = p.y + -16.0*y2*k3*k4*k4 + k1*k1;
        w.z = p.z +  -8.0*y*k4*(x4*x4 - 28.0*x4*x2*z2 + 70.0*x4*z4 - 28.0*x2*z2*z4 + z4*z4)*k1*k2;

        trap = min( trap, glm::vec4(abs(w),m) );

        m = dot(w,w);
        if( m > 256.0 )
            break;
    }

    glm::vec4 resColor = glm::vec4(m, glm::vec3(trap[1], trap[2], trap[3]));

    // distance estimation (through the Hubbard-Douady potential)
    return 0.25*log(m)*sqrt(m)/dz;
}


glm::vec4 sphere (glm::vec4 z) {
  float r2 = glm::dot(z.xyz(), z.xyz());
  if (r2 < 2.0)
    z *= (1.0 / r2);
  else z *= 0.5;

  return z;
}

// SDF box
glm::vec3 box (glm::vec3 z) {
  return glm::clamp(z, -1.0f, 1.0f) * 2.0f - z;
}

const int Iterations = 10;
const float Scale = 1.5;

float Fractal::mandelboxSDF(glm::vec3 pos) const{
    glm::vec3 c = pos;
    glm::vec3 seed = glm::vec3{0,0,0};//pos;

    float DEfactor = 1.0;
    float fixedRadius = 1;
    float fR2 = fixedRadius * fixedRadius;
    float minRadius = .5;
    float mR2 = minRadius * minRadius;
    glm::vec3 z = seed;

    float dr = 1.0;
    for (int n = 0; n < Iterations; n++) {
        //z = glm::clamp(z, -1.0f, 1.0f);
        if (z.x > 1.0)         z.x = 2.0 - z.x;
        else if (z.x < -1.0)   z.x = -2.0 - z.x;

        if (z.y > 1.0)         z.y = 2.0 - z.y;
        else if (z.y < -1.0)   z.y = -2.0 - z.y;

        if (z.z > 1.0)         z.z = 2.0 - z.z;
        else if (z.z < -1.0)   z.z = -2.0 - z.z;

        float r2 = glm::dot(z,z);

        if (r2 < mR2)
        {
            z *= fR2 / mR2;
            DEfactor *= fR2 / mR2;
        }
        else if (r2 < fR2)
        {
            z *= fR2 / r2;
            DEfactor *= fR2 / r2;
        }

        z = z * glm::abs(Scale) + c;
        DEfactor = glm::abs(Scale) * DEfactor; //+ 1;
    }
    float r = glm::dot(z,z);
    return glm::sqrt(r) / glm::abs(DEfactor);
    //return 0.25*log(r)*sqrt(r)/glm::abs(DEfactor);
}

float Fractal::serpinskiSDF(glm::vec3 z) const{
    float iterations = 15.0;
    float Scale = 2.0;
    float Offset = 3.0;
    float i = 0.0;

    float r;
    int n = 0;
    while (n < int (iterations)) {
        if (z.x + z.y < 0.0) {
            // fold 1
            float t = z.x;
            z.x = -z.y;
            z.y = -t;
        }
        if (z.x + z.z < 0.0) {
            // fold 2
            float t = z.z;
            z.z = -z.x;
            z.x = -t;
        }
        if (z.y + z.z < 0.0) {
            // fold 3
            float t = z.y;
            z.y = -z.z;
            z.z = -t;
        }
        z = z * Scale - Offset * (Scale - 1.0f);
        n++;
    }
    return (glm::length (z)) * glm::pow(Scale, -float (n));
}

float Fractal::shapeSDF(glm::vec4 p) const {
    switch(m_type){
    case FractalType::MANDELBULB:
        return mandelbulbSDF(p);
    case FractalType::MANDELBOX:
        return mandelboxSDF(p);
    case FractalType::SERPINSKI:
        return serpinskiSDF(p);
    }
}

TextureMap Fractal::getTextureMap(glm::vec4 position) const{
    position = m_ctm_inverse * position;
    // Get phi and theta angles, extrapolate from there

    float r = std::sqrt(position.x * position.x + position.z * position.z + position.y * position.y);
    float phi = std::asin(position.y / r);
    float v = phi / PI + 0.5f;
    float u;
    if(isClose(v,0) || isClose(v, 1)){
        u = 0.5f;
    } else {
        float theta = std::atan2(position.z, position.x);
        u = theta < 0 ? -theta / (2 * PI) : 1 - theta / (2 * PI);
    }

    return TextureMap{u, v};
}

glm::vec3 Fractal::getNormal(glm::vec4 position) const{
    glm::vec3 objectNormal = position;
    // Change object normal to world normal.
    return objectToWorldNormal(objectNormal, this);
}
