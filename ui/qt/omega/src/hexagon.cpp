#include "hexagon.h"

typedef Hexagon::Color Color;

Hexagon::Hexagon(const Point& center, double radius, unsigned int idx):
    center{center},
    color{Color::EMPTY},
    highlight{true},
    idx{idx}
{
    int xval, yval;
    for(int i=0; i<6; i++) {
        xval = center.x+radius*cos((0.5+i)*2*M_PI/6);
        yval = center.y+radius*sin((0.5+i)*2*M_PI/6);
        poly << QPoint(xval, yval);
    }
}

Point::Point(const double& x, const double& y):x{x}, y{y} {}

Point::Point(const Point& point): x{point.x}, y{point.y}{}

map<Color, QColor> Hexagon::colorTable = {
    {Color::WHITE, 0xDDDDDD},
    {Color::BLACK, 0x222222},
    {Color::EMPTY, 0x66CC66},
    {Color::HIGHLIGHT, 0xffff00},
};

int Hexagon::brightness = 150;

double Hexagon::inside_radius;

bool Hexagon::is_inside(const QPoint& pos) const{
    // is click position inside the interior circle of the hexagon?
    return pow(Hexagon::inside_radius,2) >= pow(pos.x()-center.x,2)+pow(pos.y()-center.y,2);
}

bool Hexagon::updateColor(const Color& color){
    if(this->color == Color::EMPTY){
        this->color = color;
        return true;
    }
    return false;
}

void Hexagon::set_inside_radius(double radius){
    inside_radius = radius*cos(M_PI/6);
}

QColor Hexagon::highlightColor() const{
    if(color == Color::EMPTY or !highlight) return Hexagon::colorTable[color];
    else{
        /* blend by simply averaging the RGBA components
         * we could simply modify only the alpha channel for drawing but
         * for that the brush object would need to be updated
        */
        QColor base = Hexagon::colorTable[color];
        QColor hColor = Hexagon::colorTable[Color::HIGHLIGHT];
        int r, g, b;
        if(color == Color::BLACK){
            r = (2*base.red()+hColor.red())/3;
            g = (2*base.green()+hColor.green())/3;
            b = (2*base.blue()+hColor.blue())/3;
        }
        else{
            r = (base.red()+2*hColor.red())/3;
            g = (base.green()+2*hColor.green())/3;
            b = (base.blue()+2*hColor.blue())/3;
        }
        return QColor(r,g,b,255);
    }
}

void Hexagon::draw(QPainter* painter) const{
    // fill hexagons with gradient pattern
    qreal xStart = poly.point(3).x();
    qreal yStart = poly.point(3).y();
    qreal xEnd = poly.point(0).x();
    qreal yEnd = poly.point(0).y();
    QLinearGradient lgrad(xStart, yStart, xEnd, yEnd);
    // highlight if necessary
    QColor fillColor = highlightColor();
    lgrad.setColorAt(0.0, fillColor.lighter(Hexagon::brightness));
    lgrad.setColorAt(1.0, fillColor.darker(Hexagon::brightness));
    QBrush brush(lgrad);
    brush.setStyle(Qt::LinearGradientPattern);
    painter->setBrush(brush);
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(poly);
}
