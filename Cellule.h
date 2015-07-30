#ifndef CELLULE_H
#define CELLULE_H

class Cellule
{

public:
    struct Coord {
        int x, y, z;

        friend bool operator==(const Coord &coord1, const Coord &coord2)
        {
            return (coord1.x == coord2.x && coord1.y == coord2.y && coord1.z == coord2.z);
        }
    };

    Cellule(Coord coord);
    Cellule(int x, int y, int z);

    Coord coord() const { return m_coord; }
    int x() const { return m_coord.x; }
    int y() const { return m_coord.y; }
    int z() const { return m_coord.z; }

private:
    Coord m_coord;

};

#endif // CELLULE_H
