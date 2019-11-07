#ifndef VALEUR_H
#define VALEUR_H

template <typename T>
class Valeur {
public:
    Valeur(T valeur);
    
    inline T getValeur() const {return m_valeur;}
    inline void setValeur(T valeur) { m_valeur = valeur; }
    
    virtual ~Valeur();
private :
        T m_valeur;
};

//class ValeurEntiere : public Valeur {
//public :
//    ValeurEntiere();
//    
//    inline int getValeur() const { return m_valeur; }
//    
//    ~ValeuEntiere();
//private :
//    int m_valeur;
//};

#endif /* VALEUR_H */

