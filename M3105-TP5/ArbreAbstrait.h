#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////

class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int executer() = 0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    void traduitEnCPP(ostream& cout, unsigned int indentation) = 0;
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
};

////////////////////////////////////////////////////////////////////////////////

class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();         // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {}       // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute chaque instruction de la séquence
    void traduitEnCPP(ostream& cout, unsigned int indentation) override;
    void ajoute(Noeud* instruction) override;  // Ajoute une instruction à la séquence

  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////

class NoeudAffectation : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {}   // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute (évalue) l'expression et affecte sa valeur à la variable
    void traduitEnCPP(ostream& cout, unsigned int indentation) override;
  private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer() override;   // Exécute (évalue) l'opération binaire)
    void traduitEnCPP(ostream& cout, unsigned int indentation) override;
  private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

class NoeudInstTantQue : public Noeud {
public :
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
    
    ~NoeudInstTantQue(){}
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) override;
private :
    Noeud* m_condition;
    Noeud* m_sequence;
};

class NoeudInstRepeter : public Noeud {
public :
    NoeudInstRepeter(Noeud* sequence, Noeud* condition);
    
    ~NoeudInstRepeter(){}
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) override;
private :
    Noeud* m_sequence;
    Noeud* m_condition;
};

class NoeudInstPour: public Noeud {
public :
    NoeudInstPour(Noeud* affectation1, Noeud* expression, Noeud* affectation2, Noeud* sequence);
    
    ~NoeudInstPour(){}
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) override;
private :
    Noeud* m_affectation1;
    Noeud* m_expression;
    Noeud* m_affectation2;
    Noeud* m_sequence;
};

class NoeudInstSiRiche: public Noeud {
public :
    NoeudInstSiRiche(std::vector<Noeud*> expressions, std::vector<Noeud*> sequences);
    
    ~NoeudInstSiRiche(){}
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) override;
private :
    std::vector<Noeud*> m_expressions;
    std::vector<Noeud*> m_sequences;
};

class NoeudInstEcrire: public Noeud {
public :
    NoeudInstEcrire(vector<Noeud*> noeuds);
    
    ~NoeudInstEcrire(){}
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) override;
private :
    vector<Noeud*> m_noeuds;
};

class NoeudInstLire: public Noeud {
public :
    NoeudInstLire(vector<Noeud*> variables);
    
    ~NoeudInstLire(){}
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) override;
private :
    vector<Noeud*> m_variables;
};

#endif /* ARBREABSTRAIT_H */
