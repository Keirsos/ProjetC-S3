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
#include "Visiteur.h"

////////////////////////////////////////////////////////////////////////////////

class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual int applique(Visiteur* v) const = 0;
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
};

////////////////////////////////////////////////////////////////////////////////

class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();         // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {}       // A cause du destructeur virtuel de la classe Noeud
    
    inline vector<Noeud *> getInstructions() const { return m_instructions; }
    void ajoute(Noeud* instruction) override;  // Ajoute une instruction à la séquence
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
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
    
    inline Noeud* getVariable() const { return m_variable; }
    inline Noeud* getExpression() const { return m_expression; }
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
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

    inline Symbole getOperateur() const { return m_operateur; }
    inline Noeud* getOperandeG() const { return m_operandeGauche; }
    inline Noeud* getOperandeD() const { return m_operandeDroit; }
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

class NoeudInstTantQue : public Noeud {
public :
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
    
    ~NoeudInstTantQue(){}
    
    inline Noeud* getCondition() const { return m_condition; }
    inline Noeud* getSequence() const { return m_sequence; }
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
private :
    Noeud* m_condition;
    Noeud* m_sequence;
};

class NoeudInstRepeter : public Noeud {
public :
    NoeudInstRepeter(Noeud* sequence, Noeud* condition);
    
    ~NoeudInstRepeter(){}
    
    inline Noeud* getCondition() const { return m_condition; }
    inline Noeud* getSequence() const { return m_sequence; }
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
private :
    Noeud* m_sequence;
    Noeud* m_condition;
};

class NoeudInstPour: public Noeud {
public :
    NoeudInstPour(Noeud* affectation1, Noeud* expression, Noeud* affectation2, Noeud* sequence);
    
    ~NoeudInstPour(){}
    
    inline Noeud* getAffect1() const { return m_affectation1; }
    inline Noeud* getExpression() const { return m_expression; }
    inline Noeud* getAffect2() const { return m_affectation2; }
    inline Noeud* getSequence() const { return m_sequence; }
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
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
    
    inline std::vector<Noeud*> getExpressions() const { return m_expressions; }
    inline std::vector<Noeud*> getSequences() const { return m_sequences; }
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
private :
    std::vector<Noeud*> m_expressions;
    std::vector<Noeud*> m_sequences;
};

class NoeudInstEcrire: public Noeud {
public :
    NoeudInstEcrire(vector<Noeud*> noeuds);
    
    ~NoeudInstEcrire(){}
    
    inline vector<Noeud*> getNoeuds() const { return m_noeuds; }
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
private :
    vector<Noeud*> m_noeuds;
};

class NoeudInstLire: public Noeud {
public :
    NoeudInstLire(vector<Noeud*> variables);
    
    ~NoeudInstLire(){}
    
    inline vector<Noeud*> getVariables() const { return m_variables; }
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
private :
    vector<Noeud*> m_variables;
};

class NoeudInstSelon: public Noeud {
public :
    NoeudInstSelon(Noeud* var, vector<Noeud*> propales, vector<Noeud*> sequences);
    
    ~NoeudInstSelon(){}
    
    inline Noeud* getVar() const { return m_var; }
    inline std::vector<Noeud*> getPropales() const { return m_propales; } 
    inline std::vector<Noeud*> getSequences() const { return m_sequences; }
    inline int applique(Visiteur* v) const override {
        return v->visite(*this);
    }
private :
    Noeud* m_var;
    vector<Noeud*> m_propales;
    vector<Noeud*> m_sequences;
};

#endif /* ARBREABSTRAIT_H */
