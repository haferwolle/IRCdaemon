#define PI 3.1415926536

extern "C" double calcCircleArea(double radius)
{
    return PI*radius*radius;
}

extern "C" double calcRectArea(double height, double width)
{
    return height * width;
}
