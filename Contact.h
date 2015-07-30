#ifndef CONTACT_H
#define CONTACT_H

#include <map>
#include <QVector3D>

#include "Cellule.h"

class Contact
{

public:
    Contact(int obj1, int obj2, Cellule::Coord coord);

    int objet1() const { return m_obj1; }
    int objet2() const { return m_obj2; }
    Cellule::Coord coord() const { return m_coord; }
    float volume() const { return m_volume; }
    float gradient(int id, unsigned int indice);
    QVector3D centre() const { return m_centre; }

    void addVolume(float value);
    void addGradient(int id, unsigned int indice, float value);
    bool equal(int obj1, int obj2, Cellule::Coord &coord);

private:
    int m_obj1, m_obj2;
    Cellule::Coord m_coord;
    float m_volume;
    std::map<unsigned int, float> m_gradient1, m_gradient2;
    QVector3D m_centre;

};

#endif // CONTACT_H
