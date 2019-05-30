//
// Created by Sam Shankland on 31/05/2019.
//

#include "Cone.h"
#include <math.h>

float Cone::intersect(glm::vec3 pos, glm::vec3 dir) {

    glm::vec3 D = pos - center;
    float yDist = height - pos.y + center.y;
    float tanHR = pow((radius / height), 2);

    // Solving for t by substituting ray equations into conic equation
    float A = pow(dir.x, 2) + pow(dir.z, 2) - (tanHR * pow(dir.y, 2));
    float B = 2*(D.x * dir.x + D.z * dir.z + tanHR * yDist * dir.y);
    float C = pow(D.x, 2) + pow(D.z, 2) - (tanHR * pow(yDist, 2));

    // Discriminant for quadratic solver
    float delta = pow(B, 2) - 4 * (A * C);

    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float termMin;
    float termMax;
    float sol1 = (-B - sqrt(delta)) / (2 * A);
    float sol2 = (-B + sqrt(delta)) / (2 * A);

    // Checking for real roots
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

    // Return closest intersection if ray is
    float yPos = pos.y + dir.y * termMin;
    if ((yPos >= center.y) && (yPos <= center.y + height)){
        return termMin;
    } else {
        yPos = pos.y + dir.y * termMax;
        if ((yPos >= center.y) && (yPos <= center.y + height)) return termMax;
        return -1.0;
    }
}

glm::vec3 Cone::normal(glm::vec3 pos) {
    glm::vec3 d = pos - center;
    float r = sqrt(d.x * d.x + d.z * d.z);
    glm::vec3 n= glm::vec3 (d.x, r*(radius/height), d.z);
    n=glm::normalize(n);
    return n;
}