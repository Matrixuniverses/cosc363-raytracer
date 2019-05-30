#include "Cylinder.h"
#include <math.h>

float Cylinder::intersect(glm::vec3 pos, glm::vec3 dir) {

    glm::vec3 D = pos - center;

    // Solving for t by substituting ray equations into cylindrical equation
    float A = pow(dir.x, 2) + pow(dir.z, 2);
    float B = 2 * (D.x * dir.x + D.z * dir.z);
    float C = pow(D.x, 2) + pow(D.z, 2) - pow(radius, 2);

    // Discriminant for quadratic solver, if discriminant is imaginary then no intersection occurred
    float delta = pow(B, 2) - 4 * (A * C);
    if(fabs(delta) < 1.e-4 || delta < 0.0) return -1;

    // Finding the solutions to the quadratic
    float termMin;
    float termMax;
    float sol1 = (-B - sqrt(delta)) / (2 * A);
    float sol2 = (-B + sqrt(delta)) / (2 * A);

    // Checking for existence of real roots (one could be imaginary)
    if(sol1 < 0.01) sol1 = -1;
    if(sol2 < 0.01) sol2 = -1;

    // Setting minimum and maximum roots
    if (sol1 > sol2){
        termMin = sol2;
        termMax = sol1;
    } else {
        termMin = sol1;
        termMax = sol2;
    }

    // Checking if the ray is intersecting at the closest point or the farthest point
    // If distance from source to intersection is minimum return the closest intersect
    // Dont return an intersection if the y pos is outside of the height y positions
    float yPos = pos.y + dir.y * termMin;
    if ((yPos >= center.y) && (yPos <= center.y + height)){
        return termMin;
    } else {
        yPos = pos.y + dir.y * termMax;
        if ((yPos >= center.y) && (yPos <= center.y + height)) return termMax;
        return -1.0;
    }
}

glm::vec3 Cylinder::normal(glm::vec3 pt) {
    glm::vec3 d = pt - center;
    return glm::normalize(glm::vec3 (d.x, 0, d.z));
}