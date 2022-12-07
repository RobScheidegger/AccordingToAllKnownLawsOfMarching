#pragma once
#include <cmath>
#include <glm/glm.hpp>
#include <vector>


/**
 * @brief bezier
 * @param points
 * @param time A [0,1] parameter identifying where we are along the bezier curve
 * @return
 */
glm::vec3 bezier(std::vector<glm::vec3>& points, float time);

glm::vec3 bezier_derivative(std::vector<glm::vec3>& points, float time);

/*
 * Binomial coefficient function borrowed from here: https://stackoverflow.com/questions/44718971/calculate-binomial-coffeficient-very-reliably
 */
size_t binom(size_t n, size_t k) noexcept;
