#pragma once
#include <cmath>

#include "glm/vec3.hpp"

// ============================= LIGHTS ====================== //

struct alignas(16) DirectionLightGPU
{
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;

    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f); float _pad1 = 0.0f;
    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);  float _pad2 = 0.0f;
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f) ;  float _pad3 = 0.0f;
    glm::vec3 specular = glm::vec3(.05f, .05f, .05f); float _pad4 = 0.0f;
};

struct alignas(16) PointLightGPU
{
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); float _pad1 = 0.0f;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float _pad2 = 0.0f;

    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);  float _pad3 = 0.0f;
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f) ;  float _pad4 = 0.0f;
    glm::vec3 specular = glm::vec3(.05f, .05f, .05f); float _pad5 = 0.0f;
};
// TODO: Fix light
struct alignas(16) SpotLightGPU
{
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); float _pad0 = 0.0f;
    glm::vec3 direction = glm::vec3(0.f, 0.f, 0.f); float _pad1 = 0.0f;

    float cosCutOff = std::cos(45.f);
    float cosOuterCutOff = std::cos(90.f);
    float dist = 0.0f;
    float _pad3 = 0.0f;

    float constant = 1.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;
    float _pad4 = 0.0f;

    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);  float _pad5= 0.0f;
    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f) ;  float _pad6 = 0.0f;
    glm::vec3 specular = glm::vec3(.05f, .05f, .05f); float _pad7 = 0.0f;
};


// ============================= LIGHTS ====================== //