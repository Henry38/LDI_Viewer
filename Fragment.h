#ifndef FRAGMENT_H
#define FRAGMENT_H

class Fragment
{
public:
    Fragment();
    Fragment(unsigned int m_object, unsigned int m_triangle, float depth, float b1, float b2, bool in);
    unsigned int idObject() const;
    unsigned int idTriangle() const;
    int i() const;
    int j() const;
    float depth() const;
    float b1() const;
    float b2() const;
    float b3() const;
    bool in() const;

    void setObject(unsigned int nObject);
    void setTriangle(unsigned int nTriangle);
    void setIJ(int i, int j);
    void setDepth(float depth);
    void setBarycentriqueCoord(float b1, float b2);
    void setIn(bool in);

    friend bool operator< (const Fragment &f1, const Fragment &f2)
    {
        if (f1.i() < f2.i()) {
            return true;
        }
        if (f1.i() > f2.i()) {
            return false;
        }
        if (f1.j() < f2.j()) {
            return true;
        }
        if (f1.j() > f2.j()) {
            return false;
        }
        return (f1.depth() > f2.depth());
    }

private:
    unsigned int m_object;      // indice de l'objet auquel le fragment est rattache
    unsigned int m_triangle;    // indice du triangle auquel le fragment est rattache
    int m_i;                    // position i du fragment sur l'image
    int m_j;                    // position j du fragment sur l'image
    float m_depth;              // profondeur du fragment
    float m_b1, m_b2;           // coordonnees barycentriques
    bool m_in;                  // fragment est-il entrant ?
};

#endif // FRAGMENT_H
