#include "Contact.h"

using namespace std;

Contact::Contact(int obj1, int obj2, Cellule::Coord coord)
{
    this->m_obj1 = obj1;
    this->m_obj2 = obj2;
    this->m_coord.x = coord.x;
    this->m_coord.y = coord.y;
    this->m_coord.z = coord.z;
    this->m_volume = 0;
    this->m_gradient1 = map<unsigned int, float>();
    this->m_gradient2 = map<unsigned int, float>();
    this->m_centre = QVector3D(0, 0, 0);
}

float Contact::gradient(int id, unsigned int indice)
{
    if (id == m_obj1) {
        return m_gradient1[indice];
    } else if (id == m_obj2) {
        return m_gradient2[indice];
    }
    return 0;
}

void Contact::addVolume(float value)
{
    this->m_volume += value;
}

void Contact::addGradient(int id, unsigned int indice, float value)
{
    if (id == m_obj1) {
        this->m_gradient1[indice] += value;
    } else if (id == m_obj2) {
        this->m_gradient2[indice] += value;
    }
}

bool Contact::equal(int obj1, int obj2, Cellule::Coord &coord)
{
    return ((obj1 == m_obj1 || obj1 == m_obj2) && (obj2 == m_obj1 || obj2 == m_obj2) && coord == m_coord);
}
