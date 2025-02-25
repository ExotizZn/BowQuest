#ifndef CAMERA_H
#define CAMERA_H

typedef struct {
    float x;
    float y;
} Camera;

void initCamera(Camera **camera);

#endif