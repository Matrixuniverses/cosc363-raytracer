/*========================================================================
* COSC363  Computer Graphics (2019)
* Ray tracer assignment
* Author: Sam Shankland (sjs227)
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cone.h"
#include "Plane.h"
#include "Cylinder.h"
#include "SceneObject.h"
#include "Ray.h"
#include "TextureBMP.h"
#include <GL/glut.h>
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 700;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

TextureBMP sphereTexture;
vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
    // Lighting constants
	glm::vec3 backgroundCol(0);
	glm::vec3 light1(5, 40, -5);
	glm::vec3 light2(-10, 60, -100);
	glm::vec3 ambientCol(0.2);

	float glassERA = 1.0f/1.05f;
	float opacity = 0.4f;

    // Calculate the closest point of intersection, if there is none then return the background colour
    ray.closestPt(sceneObjects);
    if(ray.xindex == -1) return backgroundCol;

    // Calculate color values and normal vectors of objects
    glm::vec3 materialCol = sceneObjects[ray.xindex] -> getColor(); // Colour of the scene object at point of intersect
    glm::vec3 normalVec = sceneObjects[ray.xindex] -> normal(ray.xpt); // Normal vector

    // Distance from light source (must be calculated before normalisation)
    float light1Dist = glm::length(light1 - ray.xpt);
    float light2Dist = glm::length(light2 - ray.xpt);
    glm::vec3 lightVec1 = glm::normalize(light1 - ray.xpt); // Light vector (normalized)
    glm::vec3 lightVec2 = glm::normalize(light2 - ray.xpt); // Light vector (normalized)

    // Final colour of the point after all trace steps
    glm::vec3 colSum = materialCol;

    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * LIGHTING CALCULATIONS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    // Calculate reflection vector
    glm::vec3 reflectionVec1 = glm::reflect(-lightVec1, normalVec);
    glm::vec3 reflectionVec2 = glm::reflect(-lightVec2, normalVec);
    glm::vec3 viewDir = -ray.dir;

    // Calculate the specular component
    float rDotv1 = glm::dot(reflectionVec1, viewDir);
    float rDotv2 = glm::dot(reflectionVec2, viewDir);
    float specCol1 = 0.0;
    float specCol2 = 0.0;

    if(rDotv1 >= 0) specCol1 = pow(rDotv1, 10);
    if(rDotv2 >= 0) specCol2 = pow(rDotv2, 10);


    // Calculate light direction
    float lDotn1 = glm::dot(normalVec, lightVec1);
    float lDotn2 = glm::dot(normalVec, lightVec2);


    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * PROCEDURAL TEXTURE GENERATIONS
     * Object: Plane
     * Index:  3
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    if(ray.xindex == 3){
        int xLen = (int)((ray.xpt.x + 20) / 8) % 2;
        int zLen = (int)((ray.xpt.z + 50) / 8) % 2;

        materialCol = glm::vec3(0.4, 0.1, 0.1);
        if((xLen && zLen) || (!xLen && !zLen)) materialCol = glm::vec3(0.4, 0.4, 0.4);
    }

    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * TRANSPARENCY CALCULATIONS - DOES NOT INCLUDE ANY REFRACTIVE COMPUTATIONS
     *
     * Object: Red sphere
     * Index: 1
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    if(ray.xindex == 1 && step < MAX_STEPS) {
        // Create a new ray inside of the transparent object (this is to retrieve the farthest point of intersection
        Ray internalRay(ray.xpt, ray.dir);
        internalRay.closestPt(sceneObjects);

        // Check to see if there was a point of intersection
        if(internalRay.xindex == -1) return backgroundCol;

        // Create a new ray that will be used to render all of the objects behind the transparent object
        Ray externalRay(internalRay.xpt, ray.dir);
        externalRay.closestPt(sceneObjects);

        // If there is no object then return the background colour
        if(externalRay.xindex == -1) return backgroundCol;

        // Recursively perform the trace algorithm until MAX_STEPS reached
        glm::vec3 externalSum = trace(externalRay, step + 1);
        colSum = colSum * opacity + externalSum * (1 - opacity);

        return colSum;
    }

    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * REFRACTIONS CALCULATIONS - ALL REFRACTIVE OBJECTS HAVE SOME TRANSPARENCY BY DEFAULT
     *
     * Object: White sphere
     * Index:  2
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    if(ray.xindex == 2 && step < MAX_STEPS) {
        glm::vec3 refractionDir1 = glm::refract(ray.dir, normalVec, glassERA);

        // Generate another ray for inside the refractive object using computed vector using refractive index
        Ray refractionRay1(ray.xpt, refractionDir1);
        refractionRay1.closestPt(sceneObjects);

        // Case of no object intersection for refracted ray
        if(refractionRay1.xindex == -1) return backgroundCol;

        // Calculate the new normal vectors for objects based on the exiting rays vector to other objects
        glm::vec3 normalVec2 = sceneObjects[refractionRay1.xindex] -> normal(refractionRay1.xpt);
        glm::vec3 refractionDir2 = glm::refract(refractionDir1, -normalVec2, glassERA);

        // Generate the new ray for the scene objects using the vector from the exit point of the ray,
        // and the nearest intersection with other objects
        Ray refractionRay2(refractionRay1.xpt, refractionDir2);
        refractionRay2.closestPt(sceneObjects);

        // Case of no object intersection for refracted ray
        if(refractionRay2.xindex == -1) return backgroundCol;

        // Recurse through tracing algorithm to detect more refractions
        glm::vec3 refractedColSum = trace(refractionRay2, step + 1);
        colSum = colSum * opacity + refractedColSum * (1 - opacity);

        return colSum;
    }

    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * SHADOW CALCULATIONS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    // Create shadow rays and calculate the closest point of intersection
    Ray shadow1(ray.xpt, lightVec1);
    shadow1.closestPt(sceneObjects);

    Ray shadow2(ray.xpt, lightVec2);
    shadow2.closestPt(sceneObjects);

    // If l.n < 0 then there is no specular reflection, only ambient and diffuse
    // If the shadow vector length is less than the light vector distance then the object is in shadow
    // Else calculate all of the colours of Phong's model
    if(lDotn1 < 0 || ((shadow1.xindex > -1 && shadow1.xdist < light1Dist))) {
        colSum = ambientCol * materialCol;
    } else {
        colSum = (ambientCol * materialCol + (lDotn1 * materialCol + specCol1));
    }

    if(lDotn2 < 0 || ((shadow2.xindex > -1 && shadow2.xdist < light2Dist))) {
        colSum += ambientCol * materialCol;
    } else {
        colSum += (ambientCol * materialCol + (lDotn2 * materialCol + specCol2));
    }

    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * REFLECTION CALCULATIONS
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    // Only perform reflection calculations on the first sphere
    if(ray.xindex == 0 && step < MAX_STEPS) {
        glm::vec3 reflectionDir = glm::reflect(ray.dir, normalVec);
        Ray reflectionRay(ray.xpt, reflectionDir);

        // Recursively generate the reflections, up to MAX_STEPS reflections
        // The reflection will have a dulling factor of 20%
        glm::vec3 reflectionCol = trace(reflectionRay, step + 1);
        colSum += (0.7f * reflectionCol);
    }

    return colSum;
}

/**
 * Creates a cube object given an origin point and sizes
 * @param xCoord x-coordinate of the origin
 * @param yCoord y-coordinate of the origin
 * @param zCoord z-coordinate of the origin
 * @param wdt Width of the object
 * @param dph Depth of the object
 * @param hgt Height of the object
 * @param col Colour of the object
 */
void cubeoid(float xCoord, float yCoord, float zCoord, float wdt, float dph, float hgt, glm::vec3 col)
{
    glm::vec3 A = glm::vec3(xCoord, yCoord, zCoord);
    glm::vec3 B = glm::vec3(xCoord + wdt, yCoord, zCoord);
    glm::vec3 C = glm::vec3(xCoord + wdt ,yCoord + hgt, zCoord);
    glm::vec3 D = glm::vec3(xCoord, yCoord + hgt, zCoord);
    glm::vec3 E = glm::vec3(xCoord + wdt,yCoord,zCoord - dph);
    glm::vec3 F = glm::vec3(xCoord + wdt,yCoord + hgt,zCoord - dph);
    glm::vec3 G = glm::vec3(xCoord, yCoord + hgt,zCoord - dph);
    glm::vec3 H = glm::vec3(xCoord, yCoord, zCoord - dph);

    Plane *plane1 = new Plane(A, B, C, D, col);
    Plane *plane2 = new Plane(B, E, F, C, col);
    Plane *plane3 = new Plane(E, H, G, F, col);
    Plane *plane4 = new Plane(D, G, H, A, col);
    Plane *plane5 = new Plane(D, C, F, G, col);
    Plane *plane6 = new Plane(H, E, B, A, col);

    sceneObjects.push_back(plane1);
    sceneObjects.push_back(plane2);
    sceneObjects.push_back(plane3);
    sceneObjects.push_back(plane4);
    sceneObjects.push_back(plane5);
    sceneObjects.push_back(plane6);

}

glm::vec3 antiAliasSuperSample(glm::vec3 eyePos, float pixelSize, float xPoint, float yPoint) {
    float lowerQuad = pixelSize * 0.25f;
    float upperQuad = pixelSize * 0.75f;

    glm::vec3 colSum(0);

    // Bottom left quadrant ray
    Ray ray = Ray(eyePos, glm::vec3(xPoint + lowerQuad, yPoint + lowerQuad, -EDIST));
    ray.normalize();
    colSum += trace(ray, 1);

    // Top left quadrant ray
    ray = Ray(eyePos, glm::vec3(xPoint + lowerQuad, yPoint + upperQuad, -EDIST));
    ray.normalize();
    colSum += trace(ray, 1);

    // Bottom right quadrant ray
    ray = Ray(eyePos, glm::vec3(xPoint + upperQuad, yPoint + lowerQuad, -EDIST));
    ray.normalize();
    colSum += trace(ray, 1);

    // Top right quadrant ray
    ray = Ray(eyePos, glm::vec3(xPoint + upperQuad, yPoint + upperQuad, -EDIST));
    ray.normalize();
    colSum += trace(ray, 1);

    return colSum *= glm::vec3(0.25);
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX-YMIN) / NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

		    Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
			ray.normalize();				//Normalize the direction of the ray to a unit vector

			// Use anti-aliasing
		    //glm::vec3 col = antiAliasSuperSample(eye, cellX, xp, yp);
		    glm::vec3 col = trace (ray, 1);

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glutSwapBuffers();
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

    // Create some textures
    sphereTexture = TextureBMP();

	// Create some spheres
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -120.0), 10.0, glm::vec3(0.2, 0.2, 0.2));
    Sphere *sphere2 = new Sphere(glm::vec3(7.5, -5, -60.0), 5.0, glm::vec3(1, 0, 0));
    Sphere *sphere3 = new Sphere(glm::vec3(-7.5, -2.5, -60.0), 2.5, glm::vec3(0.4, 0.4, 0.4));
    Cone *cone1 = new Cone(glm::vec3(12.5, -10.0, -90.0), 3.0, 7.5, glm::vec3(0.1, 0.2, 0.4));
    Cylinder *cylinder1 = new Cylinder(glm::vec3(-10.0, -10.0, -80.0), 4.0, 5.0, glm::vec3(1, 1, 0));

    // Create a floor plane
    Plane *floorPlane = new Plane(glm::vec3(-20, -20, -40),
            glm::vec3(20, -20, -40),
            glm::vec3(20, -20, -200),
            glm::vec3(-20, -20, -200),
            glm::vec3(0.2));

	// Add all the created object pointers to the sceneObjects stack
    sceneObjects.push_back(sphere1); // Index 0 - Blue sphere (reflections)
    sceneObjects.push_back(sphere2); // Index 1 - Red sphere
    sceneObjects.push_back(sphere3); // Index 2 - Green sphere (translucent with refraction)
    sceneObjects.push_back(floorPlane); // Index 3 - Checkered floor plane
    sceneObjects.push_back(cone1); // Index 4 - Cone
    sceneObjects.push_back(cylinder1); // Index 5 - Yellow cylinder

    // Create a rectangular prism
    cubeoid(-10, 7.5, -90.0, 4, 2, 6, glm::vec3(0.2, 0.8, 0.8));
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Ray-tracing assignment");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
