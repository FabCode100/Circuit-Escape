#ifndef CAMERA_H
#define CAMERA_H

class Camera {
public:
    Camera(int width, int height);
    void SetPosition(int x, int y);
    int GetX() const;
    int GetY() const;
    int GetWidth() const;
    int GetHeight() const;
private:
    int x, y, width, height;
};

#endif // CAMERA_H
