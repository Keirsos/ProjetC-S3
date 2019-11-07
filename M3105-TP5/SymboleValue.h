#ifndef SYMBOLEVALUE_H
#define SYMBOLEVALUE_H

#include <string.h>
#include <iostream>
using namespace std;

#include "Symbole.h"
#include "ArbreAbstrait.h"

class SymboleValue : public Symbole,  // Un symbole valué est un symbole qui a une valeur (définie ou pas)
                     public Noeud  {  //  et c'est aussi une feuille de l'arbre abstrait
public:
	  SymboleValue(const Symbole & s); // Construit un symbole valué à partir d'un symbole existant s
	  ~SymboleValue( ) {}
          
          inline bool getDefini() const { return m_defini; }
          inline int getValeur() const { return m_valeur->getValeur(); }
	  inline void setValeur(int valeur)    { this->m_valeur->setValeur(valeur); m_defini=true;  } // accesseur
	  inline bool estDefini()              { return m_defini;                       } // accesseur
          inline int applique(Visiteur* v) const override {
              return v->visite(*this);
          }
	  friend ostream & operator << (ostream & cout, const SymboleValue & symbole); // affiche un symbole value sur cout

private:
	  bool m_defini;	// indique si la valeur du symbole est définie
	  Valeur *  m_valeur;	// valeur du symbole si elle est définie, zéro sinon

};

#endif /* SYMBOLEVALUE_H */
