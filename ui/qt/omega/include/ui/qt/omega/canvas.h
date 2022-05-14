#ifndef CANVAS_H
#define CANVAS_H

#include "hexagon.h"
#include "engine/game/omega/omega.h"

#include <cmath>
#include <vector>
#include <QPainter>
#include <QFrame>
#include <QMouseEvent>
#include <QHBoxLayout>

class Canvas: public QFrame{
    Q_OBJECT
public:
    // ---- initializers ----
    Canvas(QWidget *parent, int boardSize, double radius, double padding);

    Canvas(const Canvas&)=delete;
    Canvas& operator=(const Canvas&)=delete;

    ~Canvas()=default;
    void reset();

    // ---- geometry computations ----
    vector<Point> hexa_centers() const;
    Point ref_point() const;
    Hexagon* locate_hexagon(const QPoint& pos);

    // ---- queries ----
    double board_height() const;
    double board_width() const;
    unsigned num_hexagons() const;

    // updates canvas by ai moves
    void aiBotMovedEvent(unsigned whiteIdx, unsigned blackIdx);

    // render canvas un/clickable
    bool active;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // ---- board parameters ----
    int boardSize;
    double radius;
    double padding;

    // ---- hexagons ----
    void create_hexagons();
    vector<vector<Hexagon>> hexagons;
    vector<Hexagon*> hexArray;

    // ---- variables for taking turns ----
    Hexagon::Color currentColor;
    Hexagon* prevWhite;
    Hexagon* prevBlack;

private slots:
    void mousePressEvent(QMouseEvent* ev);

signals:
    // cell and piece idx
    void updateGameState(Omega::Piece, unsigned);
};

#endif // CANVAS_H
