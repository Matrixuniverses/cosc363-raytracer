/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CONE
#define H_CONE

#include <glm/glm.hpp>
#include "SceneObject.h"

class Cone : public SceneObject {
private:
    glm::vec3 center;
    float radius;
    float height;

public:
    Cone() : center(glm::vec3(0)), radius(1), height(1) {
        color = glm::vec3(1);
    };

    Cone(glm::vec3 c, float r, float h, glm::vec3 matCol) : center(c), radius(r), height(h) {
        color = matCol;
    }


    float intersect(glm::vec3 pos, glm::vec3 dir);

    glm::vec3 normal(glm::vec3 pt);
};


#endif //!H_CODE
