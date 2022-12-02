#include "canvas.h"

typedef Hexagon::Color Color;

Canvas::Canvas(QWidget *parent, int boardSize, double radius, double padding):
    QFrame(parent),
    boardSize{boardSize},
    radius{radius},
    padding{padding},
    active{false},
    currentColor{Color::WHITE},
    prevBlack{nullptr},
    prevWhite{nullptr}
{
    this->setFixedSize(board_width(), board_height());
    create_hexagons();
    connect(this, SIGNAL (updateGameState(unsigned, unsigned)), parent, SLOT (updateGameState(unsigned, unsigned)));
}

double Canvas::board_height() const{
    return (2*boardSize-2)*2*radius*cos(M_PI/6)+2*radius+2*padding;
}

double Canvas::board_width() const{
    return (2*boardSize-1)*2*radius+2*padding;
}

unsigned Canvas::num_hexagons() const{
    unsigned numRows = 2*boardSize-1;
    return boardSize*numRows+(numRows-3)/2*((numRows-3)/2+1)+boardSize-1;
}

Point Canvas::ref_point() const{
    double x = padding+boardSize*radius;
    double y = padding+radius;
    return Point{x,y};
}

vector<Point> Canvas::hexa_centers() const
{
    // computes the center positions of each hexagon on the canvas
    vector<Point> centers;
    Point bias = ref_point();
    centers.reserve(num_hexagons());
    unsigned colCount=boardSize;
    for(unsigned row=1; row<=2*boardSize-1; ++row)
    {
        for(unsigned col = 0; col<colCount; col++)
        {
            centers.push_back(bias);
            bias.x+=2*radius;
        }
        bias.x -= 2*radius*colCount;
        if(row < boardSize)
        {
            bias.x -= radius;
            ++colCount;
        }
        else
        {
            bias.x+=radius;
            --colCount;
        }
        bias.y+=2*radius*cos(M_PI/6);
    }
    return centers;
}

void Canvas::create_hexagons()
{
    /*
     * Create the hexagon objects: hexagons[row][col]. Indexing goes
     * from the bottom left to right and then up
     */
    Hexagon::set_inside_radius(radius);
    hexagons.reserve(2*boardSize-1);
    vector<Point> centers = hexa_centers();
    hexArray = vector<Hexagon*>(num_hexagons());
    unsigned colCount = boardSize;
    unsigned idx = 0;
    for(unsigned row=0; row<2*boardSize-1; ++row)
    {
        hexagons.push_back({});
        hexagons[row].reserve(colCount);
        for(unsigned col = 0; col<colCount; ++col)
        {
            hexagons[row].push_back({centers[idx], radius, idx});
            hexArray[idx] = &hexagons[row][col];
            ++idx;
        }
        if(row < boardSize-1) ++colCount;
        else --colCount;
    }
}

Hexagon* Canvas::locate_hexagon(const QPoint& pos){
    /*
     *find hexagon in which user clicked if there is any. We could use k-d tree but we can utilize the
     *specific structure of the table and find hexagon in O(1)
     *returns the a pointer to the reference else NULL
    */

    double h_row = 2*Hexagon::inside_radius;
    double y_bias = padding+radius*(1-cos(M_PI/6));
    double y = pos.y()-y_bias;
    // we use eps to make sure there is no indexing error due to numerical precision
    double eps = 0.1* h_row;
    // if we are above the table
    if(y >= (2*boardSize-1)*h_row+radius*(1-cos(M_PI/6))-eps) return nullptr;
    // if we are below the table
    if(y <= eps) return nullptr;

    int row = ceil(y/h_row);
    double x_bias = padding+std::abs(boardSize-row)*radius;
    double x = pos.x()-x_bias;
    // if we are to the left of the table
    if(x<=radius*(1-cos(M_PI/6))+eps) return nullptr;
    // if we are to the right of the table
    if(x>=2*radius*((2*boardSize-1)-abs(boardSize-row))-radius*(1-cos(M_PI/6))-eps) return nullptr;

    unsigned col = ceil(x/(2*radius));
    // location is in the high-th row in the table
    if(hexagons[row-1][col-1].is_inside(pos)) return &hexagons[row-1][col-1];
    else return nullptr;
}

void Canvas::mousePressEvent(QMouseEvent* ev)
{
    if(active){ // if game has started
        Hexagon* hexagon = locate_hexagon(ev->pos());
        unsigned pieceIdx = (unsigned) currentColor;
        // Switch color and highlight
        if(hexagon and hexagon->updateColor(currentColor)){
            if(currentColor == Color::WHITE){
                // remove highlight color from previous hexagons
                if(prevWhite and prevBlack){

                    prevWhite->highlight = false;
                    prevBlack->highlight = false;
                }
                prevWhite = hexagon;
                currentColor = Color::BLACK;
            }
            else{
                prevBlack = hexagon;
                currentColor = Color::WHITE;
            }
            // schedule paintEvent
            repaint();
            emit updateGameState(pieceIdx, hexagon->idx);
        }
    }
}

void Canvas::aiBotMovedEvent(unsigned whiteIdx, unsigned blackIdx){
    hexArray[whiteIdx]->updateColor(Color::WHITE);
    hexArray[blackIdx]->updateColor(Color::BLACK);
    // remove highlight color from previous hexagons
    if(prevWhite and prevBlack){
        prevWhite->highlight = false;
        prevBlack->highlight = false;
    }
    prevWhite = hexArray[whiteIdx];
    prevBlack = hexArray[blackIdx];
    repaint();
}

void Canvas::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    for(const vector<Hexagon>& hexagonRow : hexagons){
        for(const Hexagon& hexagon : hexagonRow)
            hexagon.draw(&painter);
    }
}

void Canvas::reset(){
    for(Hexagon* hex : hexArray){
        hex->color = Color::EMPTY;
        hex->highlight = true;
    }
    currentColor = Color::WHITE;
    prevBlack = nullptr;
    prevWhite = nullptr;
    repaint();
}
