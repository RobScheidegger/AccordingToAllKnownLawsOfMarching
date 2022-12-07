#include "utils/bezierfuncs.h"


/*
 * Binomial coefficient function borrowed from here: https://stackoverflow.com/questions/44718971/calculate-binomial-coffeficient-very-reliably
 */
size_t binom(size_t n, size_t k) noexcept
{
    return
      (        k> n  )? 0 :          // out of range
      (k==0 || k==n  )? 1 :          // edge
      (k==1 || k==n-1)? n :          // first
      (     k+k < n  )?              // recursive:
      (binom(n-1,k-1) * n)/k :       //  path to k=1   is faster
      (binom(n-1,k) * n)/(n-k);      //  path to k=n-1 is faster
}

/**
 * @brief bezier
 * @param points
 * @param time A [0,1] parameter identifying where we are along the bezier curve
 * @return
 */
glm::vec3 bezier(std::vector<glm::vec3>& points, float time){
    int n = points.size() - 1;
    glm::vec3 b {0,0,0};
    for(int i = 0; i < points.size(); i++){
        float coeff = binom(n, i) * std::pow(1 - time, n - i) * std::pow(time, i);
        b += coeff * points[i];
    }
    return b;
}

#define DELTA 0.02f
glm::vec3 bezier_derivative(std::vector<glm::vec3>& points, float time){
    return (bezier(points, time + DELTA) - bezier(points, time)) / DELTA;
}
