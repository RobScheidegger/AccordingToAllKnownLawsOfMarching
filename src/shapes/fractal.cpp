#include "fractal.h"

std::optional<Intersect> Fractal::intersect(Ray ray) const{

    // Assumes that ray is now in _object_ space, so we can just intersect with the traditional sphere.
    // Centered at origin with radius 1/2
    std::optional<Intersect> intersect = std::nullopt;

    float a = glm::dot(ray.d, ray.d);
    float b = 2 * glm::dot(ray.p, ray.d);
    float c = glm::dot(ray.p, ray.p) - 1.25; // extra - 1 from dot product being weird with 4-vectors

    std::pair<std::optional<float>, std::optional<float>> t = solveQuadratic(a, b, c);
    if(t.first.has_value()){
        replaceIntercept(intersect,
                    Intersect{this, t.first.value(), getNormal(ray.evaluate(t.first.value()))}
               );
    }
    if(t.second.has_value()){
        replaceIntercept(intersect,
                    Intersect{this, t.second.value(), getNormal(ray.evaluate(t.second.value()))}
               );
    }

    return intersect;
}

float Fractal::shapeSDF(glm::vec4 p) const {
    glm::vec3 w = p;
    float m = dot(w,w);

    glm::vec4 trap = glm::vec4(abs(w),m);
    float dz = 1.0;

    for( int i=0; i< 4; i++ ) {
//    #if 0
//            // polynomial version (no trigonometrics, but MUCH slower)
//            float m2 = m*m;
//            float m4 = m2*m2;
//            dz = 8.0*sqrt(m4*m2*m)*dz + 1.0;

//            float x = w.x; float x2 = x*x; float x4 = x2*x2;
//            float y = w.y; float y2 = y*y; float y4 = y2*y2;
//            float z = w.z; float z2 = z*z; float z4 = z2*z2;

//            float k3 = x2 + z2;
//            float k2 = inversesqrt( k3*k3*k3*k3*k3*k3*k3 );
//            float k1 = x4 + y4 + z4 - 6.0*y2*z2 - 6.0*x2*y2 + 2.0*z2*x2;
//            float k4 = x2 - y2 + z2;

//            w.x = p.x +  64.0*x*y*z*(x2-z2)*k4*(x4-6.0*x2*z2+z4)*k1*k2;
//            w.y = p.y + -16.0*y2*k3*k4*k4 + k1*k1;
//            w.z = p.z +  -8.0*y*k4*(x4*x4 - 28.0*x4*x2*z2 + 70.0*x4*z4 - 28.0*x2*z2*z4 + z4*z4)*k1*k2;
//    #else
        // trigonometric version (MUCH faster than polynomial)

        // dz = 8*z^7*dz
        dz = 8.0*pow(m,3.5)*dz + 1.0;

        // z = z^8+c
        float r = length(w);
        float b = 8.0*acos( w.y/r);
        float a = 8.0*glm::atan( w[0], w[1] );
        w = glm::vec3(p) + powf(r, 8.f) * glm::vec3( sin(b)*sin(a), cos(b), sin(b)*cos(a) );
//    #endif

        trap = min( trap, glm::vec4(abs(w),m) );

        m = dot(w,w);
        if( m > 256.0 ) {
            break;
        }

    }
    // distance estimation (through the Hubbard-Douady potential)
    return 0.25*log(m)*sqrt(m)/dz;
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
