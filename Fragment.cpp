#include "Fragment.h"

Fragment::Fragment()
{
    this->m_object = 0;
    this->m_triangle = 0;
    this->m_depth = 0;
    this->m_b1 = 0;
    this->m_b2 = 0;
    this->m_in = false;
}

Fragment::Fragment(unsigned int nObject, unsigned int nTriangle, float depth, float b1, float b2, bool in)
{
    this->m_object = nObject;
    this->m_triangle = nTriangle;
    this->m_depth = depth;
    this->m_b1 = b1;
    this->m_b2 = b2;
    this->m_in = in;
}

unsigned int Fragment::idObject() const
{
    return this->m_object;
}

unsigned int Fragment::idTriangle() const
{
    return this->m_triangle;
}

int Fragment::i() const
{
    return m_i;
}

int Fragment::j() const
{
    return m_j;
}
float Fragment::depth() const
{
    return this->m_depth;
}

float Fragment::b1() const
{
    return m_b1;
}

float Fragment::b2() const
{
    return m_b2;
}

float Fragment::b3() const
{
    return (1.0f - m_b1 - m_b2);
}

bool Fragment::in() const
{
    return this->m_in;
}

void Fragment::setObject(unsigned int nObject)
{
    this->m_object = nObject;
}

void Fragment::setTriangle(unsigned int nTriangle)
{
    this->m_triangle = nTriangle;
}

void Fragment::setIJ(int i, int j)
{
    this->m_i = i;
    this->m_j = j;
}

void Fragment::setDepth(float depth)
{
    this->m_depth = depth;
}

void Fragment::setBarycentriqueCoord(float b1, float b2)
{
    this->m_b1 = b1;
    this->m_b2 = b2;
}

void Fragment::setIn(bool in)
{
    this->m_in = in;
}
