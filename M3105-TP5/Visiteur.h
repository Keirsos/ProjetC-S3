#ifndef VISITEUR_H
#define VISITEUR_H

#include <iostream>

class Visiteur {
public:    
    virtual int visite(class NoeudSeqInst& n) const = 0;
    virtual int visite(class NoeudAffectation& n) const = 0;
    virtual int visite(class NoeudOperateurBinaire& n) const = 0;
    virtual int visite(class NoeudInstTantQue& n) const = 0;
    virtual int visite(class NoeudInstRepeter& n) const = 0;
    virtual int visite(class NoeudInstPour& n) const = 0;
    virtual int visite(class NoeudInstSiRiche& n) const = 0;
    virtual int visite(class NoeudInstEcrire& n) const = 0;
    virtual int visite(class NoeudInstLire& n) const = 0;
    virtual int visite(class NoeudInstSelon& n) const = 0;
    virtual int visite(class SymboleValue& n) const = 0;
    
    virtual ~Visiteur();
};

class Executer : public Visiteur {
public :
    Executer();
    
    int visite(NoeudSeqInst& n) const override;
    int visite(NoeudAffectation& n) const override;
    int visite(NoeudOperateurBinaire& n) const override;
    int visite(NoeudInstTantQue& n) const override;
    int visite(NoeudInstRepeter& n) const override;
    int visite(NoeudInstPour& n) const override;
    int visite(NoeudInstSiRiche& n) const override;
    int visite(NoeudInstEcrire& n) const override;
    int visite(NoeudInstLire& n) const override;
    int visite(NoeudInstSelon& n) const override;
    int visite(SymboleValue& n) const override;
    
    ~Executer();
};

class TraduitEnCPP : public Visiteur {
public :
    TraduitEnCPP(std::ostream& cout, unsigned int indentation, bool pointVirgule = true);
    
    int visite(NoeudSeqInst& n) const override;
    int visite(NoeudAffectation& n) const override;
    int visite(NoeudOperateurBinaire& n) const override;
    int visite(NoeudInstTantQue& n) const override;
    int visite(NoeudInstRepeter& n) const override;
    int visite(NoeudInstPour& n) const override;
    int visite(NoeudInstSiRiche& n) const override;
    int visite(NoeudInstEcrire& n) const override;
    int visite(NoeudInstLire& n) const override;
    int visite(NoeudInstSelon& n) const override;
    int visite(SymboleValue& n) const override;
    
    ~TraduitEnCPP();
private :
    std::ostream& cout;
    unsigned int indentation;
    bool pointVirgule;
};

#endif /* VISITEUR_H */