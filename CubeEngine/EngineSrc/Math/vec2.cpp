#include "vec2.h"


namespace tzw {

vec2::vec2()
    :x(0.0f),y(0.0f)
{

}

vec2::vec2(float the_x, float the_y)
    :x(the_x),y(the_y)
{

}

void vec2::set(float the_x, float the_y)
{
    x = the_x;
    y = the_y;
}

vec2 vec2::operator +(const vec2 &other) const
{
    return vec2(x + other.x,y + other.y);
}

vec2 vec2::operator -(const vec2 &other) const
{
    return vec2(x - other.x,y - other.y);
}

vec2 vec2::operator /(const float &other) const
{
    return vec2(x / other, y / other);
}
float vec2::getY() const
{
    return y;
}

void vec2::setY(float value)
{
    y = value;
}

float vec2::getX() const
{
    return x;
}

void vec2::setX(float value)
{
    x = value;
}




} // namespace tzw

