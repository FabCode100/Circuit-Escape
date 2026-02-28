#include "Camera.h"

Camera::Camera(int width, int height) : x(0), y(0), width(width), height(height) {}
void Camera::SetPosition(int x_, int y_) { x = x_; y = y_; }
int Camera::GetX() const { return x; }
int Camera::GetY() const { return y; }
int Camera::GetWidth() const { return width; }
int Camera::GetHeight() const { return height; }
