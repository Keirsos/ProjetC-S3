#ifndef VISITEUR_H
#define VISITEUR_H

#include <iostream>

class Visiteur {
public:    
    virtual int visite(const class NoeudSeqInst& n) const = 0;
    virtual int visite(const class NoeudAffectation& n) const = 0;
    virtual int visite(const class NoeudOperateurBinaire& n) const = 0;
    virtual int visite(const class NoeudInstTantQue& n) const = 0;
    virtual int visite(const class NoeudInstRepeter& n) const = 0;
    virtual int visite(const class NoeudInstPour& n) const = 0;
    virtual int visite(const class NoeudInstSiRiche& n) const = 0;
    virtual int visite(const class NoeudInstEcrire& n) const = 0;
    virtual int visite(const class NoeudInstLire& n) const = 0;
    virtual int visite(const class NoeudInstSelon& n) const = 0;
    virtual int visite(const class SymboleValue& n) const = 0;
    
    virtual ~Visiteur();
};

class Executer : public Visiteur {
public :
    Executer();
    
    int visite(const NoeudSeqInst& n) const override;
    int visite(const NoeudAffectation& n) const override;
    int visite(const NoeudOperateurBinaire& n) const override;
    int visite(const NoeudInstTantQue& n) const override;
    int visite(const NoeudInstRepeter& n) const override;
    int visite(const NoeudInstPour& n) const override;
    int visite(const NoeudInstSiRiche& n) const override;
    int visite(const NoeudInstEcrire& n) const override;
    int visite(const NoeudInstLire& n) const override;
    int visite(const NoeudInstSelon& n) const override;
    int visite(const SymboleValue& n) const override;
    
    ~Executer();
};

class TraduitEnCPP : public Visiteur {
public :
    TraduitEnCPP(std::ostream& cout, unsigned int indentation, bool pointVirgule = true);
    
    int visite(const NoeudSeqInst& n) const override;
    int visite(const NoeudAffectation& n) const override;
    int visite(const NoeudOperateurBinaire& n) const override;
    int visite(const NoeudInstTantQue& n) const override;
    int visite(const NoeudInstRepeter& n) const override;
    int visite(const NoeudInstPour& n) const override;
    int visite(const NoeudInstSiRiche& n) const override;
    int visite(const NoeudInstEcrire& n) const override;
    int visite(const NoeudInstLire& n) const override;
    int visite(const NoeudInstSelon& n) const override;
    int visite(const SymboleValue& n) const override;
    
    ~TraduitEnCPP();
private :
    std::ostream& cout;
    unsigned int indentation;
    bool pointVirgule;
};

#endif /* VISITEUR_H */