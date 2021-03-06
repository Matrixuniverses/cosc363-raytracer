#include "Cone.h"
#include <math.h>

/**
 * Checks for a ray intersection with the cone using a quadratic solver for each of the two intersection points
 * @param pos Ray position
 * @param dir Ray direction
 * @return -1 if no intersection, closest point of intersection relative to the position of the ray otherwise
 * @author sjs227
 */
float Cone::intersect(glm::vec3 pos, glm::vec3 dir) {

    glm::vec3 D = pos - center;
    float yDist = height - pos.y + center.y;
    float tanHR = pow((radius / height), 2);

    // Solving for t by substituting ray equations into conic equation
    float A = pow(dir.x, 2) + pow(dir.z, 2) - (tanHR * pow(dir.y, 2));
    float B = 2 * (D.x * dir.x + D.z * dir.z + tanHR * yDist * dir.y);
    float C = pow(D.x, 2) + pow(D.z, 2) - (tanHR * pow(yDist, 2));

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

/**
 * Compute the normal vector at a given position
 * @param pos Position of normal
 * @return normalized glm::vec3 of the surface normal
 */
glm::vec3 Cone::normal(glm::vec3 pt) {
    glm::vec3 d = pt - center;
    float r = sqrt(d.x * d.x + d.z * d.z);
    return glm::normalize(glm::vec3 (d.x, r * (radius / height), d.z));
}