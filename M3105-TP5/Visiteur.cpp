#include "Visiteur.h"
#include "SymboleValue.h"

using namespace std;

Visiteur::~Visiteur() {}


Executer::Executer(){}

int Executer::visite(NoeudSeqInst& n) const{
    for (unsigned int i = 0; i < n.getInstructions().size(); i++)
        n.getInstructions()[i]->applique(this);
        //n.getInstructions()[i]->applique(this); // on exécute chaque instruction de la séquence
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(NoeudAffectation& n) const {
    int valeur = n.getExpression()->applique(this); // On exécute (évalue) l'expression
    ((SymboleValue*) n.getVariable())->setValeur(valeur); // On affecte la variable
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(NoeudOperateurBinaire& n) const {
    int og, od, valeur;
    if (n.getOperandeG() != nullptr) og = n.getOperandeG()->applique(this); // On évalue l'opérande gauche
    if (n.getOperandeD() != nullptr) od = n.getOperandeD()->applique(this); // On évalue l'opérande droit
    // Et on combine les deux opérandes en fonctions de l'opérateur
    if (n.getOperateur() == "+") valeur = (og + od);
    else if (n.getOperateur() == "-") valeur = (og - od);
    else if (n.getOperateur() == "*") valeur = (og * od);
    else if (n.getOperateur() == "==") valeur = (og == od);
    else if (n.getOperateur() == "!=") valeur = (og != od);
    else if (n.getOperateur() == "<") valeur = (og < od);
    else if (n.getOperateur() == ">") valeur = (og > od);
    else if (n.getOperateur() == "<=") valeur = (og <= od);
    else if (n.getOperateur() == ">=") valeur = (og >= od);
    else if (n.getOperateur() == "et") valeur = (og && od);
    else if (n.getOperateur() == "ou") valeur = (og || od);
    else if (n.getOperateur() == "non") valeur = (!og);
    else if (n.getOperateur() == "/") {
        if (od == 0) throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}
int Executer::visite(NoeudInstTantQue& n) const {
    while (n.getCondition()->applique(this)) n.getSequence()->applique(this);
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(NoeudInstRepeter& n) const {
    if(!n.getCondition()->applique(this)){
        do
            n.getSequence()->applique(this);
        while(!n.getCondition()->applique(this));
    }
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(NoeudInstPour& n) const {
    if(n.getAffect1() != nullptr) n.getAffect1()->applique(this);
    while(n.getExpression()->applique(this)){
        n.getSequence()->applique(this);
        if(n.getAffect2() != nullptr) n.getAffect2()->applique(this);
    }
    
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(NoeudInstSiRiche& n) const {
    int i = 0;
    while(i < n.getExpressions().size() && !(n.getExpressions().at(i)->applique(this))){
        i++;
    }
    if (i < n.getExpressions().size()) n.getSequences().at(i)->applique(this);
    else                               n.getSequences().at(n.getSequences().size()-1)->applique(this);
    
    return 0;
}
int Executer::visite(NoeudInstEcrire& n) const {
    for(Noeud* noeud : n.getNoeuds()){
        if((typeid(*noeud)==typeid(SymboleValue) &&  *((SymboleValue*)noeud)== "<CHAINE>" )){
            string chaine = ((SymboleValue*)noeud)->getChaine();
            chaine.erase(0,1); chaine.erase(chaine.size()-1,1);
            cout << chaine;
        }
        else{
            cout << noeud->applique(this);
        }
    }
    cout << endl;
    
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(NoeudInstLire& n) const {
    for(Noeud* var : n.getVariables()){
        int temp;
        cin >> temp;
        ((SymboleValue*)var)->setValeur(temp);
    }
    
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(NoeudInstSelon& n) const {
    int i = 0;
    while(i < n.getPropales().size() && n.getVar() != n.getPropales()[i]){
        i++;
    }
    if (i < n.getPropales().size())  n.getSequences()[i]->applique(this);
    else                        n.getSequences()[n.getSequences().size()-1]->applique(this);
    
    return 0;
}
int Executer::visite(SymboleValue& n) const {
    if (!n.getDefini()) throw IndefiniException(); // on lève une exception si valeur non définie
    return n.getValeur();
}


TraduitEnCPP::TraduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) :
    cout(cout),
    indentation(indentation),
    pointVirgule(pointVirgule)
{}

int TraduitEnCPP::visite(NoeudSeqInst& n) const{
    for(Noeud* noeud : n.getInstructions()){
        noeud->applique(TraduitEnCPP(cout,indentation));
        cout << endl;
    }
}
int TraduitEnCPP::visite(NoeudAffectation& n) const{
    cout << setw(4*indentation) << "";
    n.getVariable()->applique(TraduitEnCPP(cout,0));
    cout << " = " << "";
    n.getExpression()->applique(TraduitEnCPP(cout,0));
    if(pointVirgule) cout << ";";
}
int TraduitEnCPP::visite(NoeudOperateurBinaire& n) const{
    cout << setw(4*indentation) << "";
    cout << "(";
    n.getOperandeG()->applique(TraduitEnCPP(cout,0));
    string operateur = n.getOperateur().getChaine();
    if (operateur == "et")      operateur = "&&";
    else if (operateur == "ou") operateur = "||";
    cout << " " << operateur << " ";
    n.getOperandeD()->applique(TraduitEnCPP(cout,0));
    cout << ")";
}
int TraduitEnCPP::visite(NoeudInstTantQue& n) const{
    cout << setw(4*indentation) << "" << "while(";
    n.getCondition()->applique(TraduitEnCPP(cout,0));
    cout << ") {" << endl;
    n.getSequence()->applique(TraduitEnCPP(cout, indentation+1));
    cout << setw(4*indentation) << "" << "}";
}
int TraduitEnCPP::visite(NoeudInstRepeter& n) const{
    cout << setw(4*indentation) << "" << "if(!(";
    n.getCondition()->applique(TraduitEnCPP(cout,0));
    cout << ")) {" << endl << setw(4*(indentation+1)) << "" << "do" << endl;
    n.getSequence()->applique(TraduitEnCPP(cout,indentation+2));
    cout << setw(4*(indentation+1)) << "" << "while(!(";
    n.getCondition()->applique(TraduitEnCPP(cout,0));
    cout << "));" << endl << setw(4*indentation) << "" << "}";
}
int TraduitEnCPP::visite(NoeudInstPour& n) const{
    cout << setw(4*indentation) << "" << "for(";
    if(n.getAffect1() != nullptr) n.getAffect1()->applique(TraduitEnCPP(cout,0,false));
    cout << "" << " ; ";
    n.getExpression()->applique(TraduitEnCPP(cout,0));
    cout << "" << " ; ";
    if(n.getAffect2() != nullptr) n.getAffect2()->applique(TraduitEnCPP(cout,0,false));
    cout << "" << ") {" << endl;
    n.getSequence()->applique(TraduitEnCPP(cout,indentation+1));
    cout << setw(4*indentation) << "" << "}";
}
int TraduitEnCPP::visite(NoeudInstSiRiche& n) const{
    cout << setw(4*indentation) << "" << "if(";
    n.getExpressions()[0]->applique(TraduitEnCPP(cout,0));
    cout << ") {" << endl;
    n.getSequences()[0]->applique(TraduitEnCPP(cout,indentation+1));
    cout << setw(4*indentation) << "" << "}";
    for(int i = 1; i<n.getExpressions().size(); i++){
        cout << endl << setw(4*indentation) << "" << "else if(";
        n.getExpressions()[i]->applique(TraduitEnCPP(cout,0));
        cout << ") {" << endl;
        n.getSequences()[i]->applique(TraduitEnCPP(cout, indentation+1));
        cout << setw(4*indentation) << "" << "}";
    }
    if(n.getSequences().size() > n.getExpressions().size()){
        cout << endl << setw(4*indentation) << "" << "else {" << endl;
        n.getSequences()[n.getSequences().size()-1]->applique(TraduitEnCPP(cout, indentation+1));
        cout << setw(4*indentation) << "" << "}";
    }
}
int TraduitEnCPP::visite(NoeudInstEcrire& n) const{
    cout << setw(4*indentation) << "" << "cout";
    for(Noeud* noeud : n.getNoeuds()){
        cout << " << ";
        noeud->applique(TraduitEnCPP(cout,0));
    }
    cout << " << endl;";
}
int TraduitEnCPP::visite(NoeudInstLire& n) const{
    cout << setw(4*indentation) << "" << "cin";
    for(Noeud* variables : n.getVariables()){
        cout << " >> ";
        variables->applique(TraduitEnCPP(cout,0));
        cout << ";";
    }
}
int TraduitEnCPP::visite(NoeudInstSelon& n) const{
    cout << setw(4*indentation) << "" << "switch(";
    n.getVar()->applique(TraduitEnCPP(cout,0));
    cout << ") {" << endl;
    for(int i = 0; i < n.getPropales().size(); i++){
        cout << setw(4*(indentation+1)) << "" << "case ";
        n.getPropales()[i]->applique(TraduitEnCPP(cout,indentation+1));
        cout << " :" << endl;
        n.getSequences()[i]->applique(TraduitEnCPP(cout, indentation+2));
        cout << setw(4*(indentation+2)) << "" << "break;" << endl;
    }
    if(n.getSequences().size() > n.getPropales().size()){
        cout << endl << setw(4*(indentation+1)) << "" << "default :" << endl;
        n.getSequences()[n.getSequences().size()-1]->applique(TraduitEnCPP(cout, indentation+2));
    }
    cout << setw(4*indentation) << "" << "}";
}
int TraduitEnCPP::visite(SymboleValue& n) const{
    cout << setw(4*indentation) << n.getChaine();
}

