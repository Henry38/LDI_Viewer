#include "Cellule.h"


Cellule::Cellule(Cellule::Coord coord)
{
    this->m_coord = coord;
}

Cellule::Cellule(int x, int y, int z)
{
    this->m_coord.x = x;
    this->m_coord.y = y;
    this->m_coord.z = z;
}
