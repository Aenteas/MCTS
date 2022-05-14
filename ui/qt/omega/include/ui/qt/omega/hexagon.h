#ifndef HEXAGON_H
#define HEXAGON_H

#include <math.h>
#include <map>
#include <QColor>
#include <QPoint>
#include <QPainter>
#include <QPolygon>
#include <QLinearGradient>

using namespace std;

struct Point{
    double x, y;
    Point(const double& x, const double& y);
    Point(const Point& point);
    Point();
};

class Hexagon
{
    friend class Canvas;
public:
    enum Color{WHITE, BLACK, EMPTY, HIGHLIGHT};
    Hexagon(const Point& center, double radius, unsigned int idx);

    ~Hexagon()=default;

    Hexagon(const Hexagon&)=default;
    Hexagon& operator=(const Hexagon&)=delete;

    bool is_inside(const QPoint& pos) const;
    void draw(QPainter* painter) const;

private:
    // vertex coordinates of hexagons
    QPolygon poly;
    // radius of interior circle of hexagons
    static void set_inside_radius(double radius);
    static double inside_radius;
    bool updateColor(const Color& color);
    // center coordinate of the hexagon
    Point center;
    // color of the hexagon
    Color color;
    bool highlight;
    // brigthness factor that highilghts the most recent moves
    static int brightness;
    // hexa codes for colors
    static map<Color, QColor> colorTable;
    QColor highlightColor() const;
    // indices of hexagons (row-by-row from left to right from top to bottom)
    unsigned int idx;
};

#endif // HEXAGON_H
