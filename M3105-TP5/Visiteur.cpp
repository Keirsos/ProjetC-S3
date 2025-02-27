#include "Visiteur.h"
#include "SymboleValue.h"

using namespace std;

Visiteur::~Visiteur() {}


Executer::Executer(){}

int Executer::visite(const NoeudSeqInst& n) const{
    for (unsigned int i = 0; i < n.getInstructions().size(); i++){
        Visiteur * v = new Executer;
        n.getInstructions()[i]->applique(v);
        delete(v);
    }
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(const NoeudAffectation& n) const {
    Visiteur * v = new Executer;
    int valeur = n.getExpression()->applique(v); // On exécute (évalue) l'expression
    delete(v);
    ((SymboleValue*) n.getVariable())->setValeur(valeur); // On affecte la variable
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(const NoeudOperateurBinaire& n) const {
    int og, od, valeur;
    Visiteur * v = new Executer;
    if (n.getOperandeG() != nullptr) og = n.getOperandeG()->applique(v); // On évalue l'opérande gauche
    if (n.getOperandeD() != nullptr) od = n.getOperandeD()->applique(v); // On évalue l'opérande droit
    delete(v);
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
int Executer::visite(const NoeudInstTantQue& n) const {
    Visiteur * v = new Executer;
    while (n.getCondition()->applique(v)) n.getSequence()->applique(v);
    delete(v);
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(const NoeudInstRepeter& n) const {
    Visiteur * v = new Executer;
    if(!n.getCondition()->applique(v)){
        do
            n.getSequence()->applique(v);
        while(!n.getCondition()->applique(v));
    }
    delete(v);
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(const NoeudInstPour& n) const {
    Visiteur * v = new Executer;
    if(n.getAffect1() != nullptr) n.getAffect1()->applique(v);
    while(n.getExpression()->applique(v)){
        n.getSequence()->applique(v);
        if(n.getAffect2() != nullptr) n.getAffect2()->applique(v);
    }
    delete(v);
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(const NoeudInstSiRiche& n) const {
    
    Visiteur * v = new Executer;
    int i = 0;
    while(i < n.getExpressions().size() && !(n.getExpressions().at(i)->applique(v))){ //On cherche une condition vérifiée
        i++;
    }
    if (i < n.getExpressions().size()){ // si on a trouvé une condition true
        n.getSequences()[i]->applique(v); // on l'exécute
    }
    else {
        if (n.getSequences().size() > n.getExpressions().size()){ // si on a rien trouvé de true, on vérifie si il y a un sinon
            n.getSequences()[n.getSequences().size()-1]->applique(v); // si oui on l'exécute
        } // si non, on a terminé le if et on a rien à exécuter
    }
    delete(v);
    
    return 0;
}
int Executer::visite(const NoeudInstEcrire& n) const {
    for(Noeud* noeud : n.getNoeuds()){
        if((typeid(*noeud)==typeid(SymboleValue) &&  *((SymboleValue*)noeud)== "<CHAINE>" )){
            string chaine = ((SymboleValue*)noeud)->getChaine();
            chaine.erase(0,1); chaine.erase(chaine.size()-1,1);
            cout << chaine;
        }
        else{
            Visiteur * v = new Executer;
            cout << noeud->applique(v);
            delete(v);
        }
    }
    cout << endl;
    
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(const NoeudInstLire& n) const {
    for(Noeud* var : n.getVariables()){
        int temp;
        cin >> temp;
        ((SymboleValue*)var)->setValeur(temp);
    }
    
    return 0; // La valeur renvoyée ne représente rien !
}
int Executer::visite(const NoeudInstSelon& n) const {
    int i = 0;
    while(i < n.getPropales().size() && n.getVar() != n.getPropales()[i]){
        i++;
    }
    Visiteur * v = new Executer;
    if (i < n.getPropales().size())  n.getSequences()[i]->applique(v);
    else                        n.getSequences()[n.getSequences().size()-1]->applique(v);
    delete(v);
    return 0;
}
int Executer::visite(const SymboleValue& n) const {
    if (!n.getDefini()) throw IndefiniException(); // on lève une exception si valeur non définie
    return n.getValeur();
}

Executer::~Executer(){}


TraduitEnCPP::TraduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) :
    cout(cout),
    indentation(indentation),
    pointVirgule(pointVirgule)
{}

int TraduitEnCPP::visite(const NoeudSeqInst& n) const{
    for(Noeud* noeud : n.getInstructions()){
        Visiteur * v = new TraduitEnCPP(cout, indentation);
        noeud->applique(v);
        delete(v);
        cout << endl;
    }
    return 0;
}
int TraduitEnCPP::visite(const NoeudAffectation& n) const{
    cout << setw(4*indentation) << "";
    Visiteur * v = new TraduitEnCPP(cout, 0);
    n.getVariable()->applique(v);
    cout << " = " << "";
    n.getExpression()->applique(v);
    delete(v);
    if(pointVirgule) cout << ";";
    return 0;
}
int TraduitEnCPP::visite(const NoeudOperateurBinaire& n) const{
    cout << setw(4*indentation) << "";
    cout << "(";
    Visiteur * v = new TraduitEnCPP(cout, 0);
    n.getOperandeG()->applique(v);
    string operateur = n.getOperateur().getChaine();
    if (operateur == "et")      operateur = "&&";
    else if (operateur == "ou") operateur = "||";
    cout << " " << operateur << " ";
    n.getOperandeD()->applique(v);
    delete(v);
    cout << ")";
    return 0;
}
int TraduitEnCPP::visite(const NoeudInstTantQue& n) const{
    cout << setw(4*indentation) << "" << "while(";
    Visiteur * v = new TraduitEnCPP(cout, 0);
    n.getCondition()->applique(v);
    delete(v);
    cout << ") {" << endl;
    Visiteur * v2 = new TraduitEnCPP(cout, indentation+1);
    n.getSequence()->applique(v2);
    delete(v2);
    cout << setw(4*indentation) << "" << "}";
    return 0;
}
int TraduitEnCPP::visite(const NoeudInstRepeter& n) const{
    cout << setw(4*indentation) << "" << "if(!(";
    Visiteur * v = new TraduitEnCPP(cout, 0);
    n.getCondition()->applique(v);
    cout << ")) {" << endl << setw(4*(indentation+1)) << "" << "do" << endl;
    Visiteur * v2 = new TraduitEnCPP(cout, indentation+2);
    n.getSequence()->applique(v2);
    delete(v2);
    cout << setw(4*(indentation+1)) << "" << "while(!(";
    n.getCondition()->applique(v);
    delete(v);
    cout << "));" << endl << setw(4*indentation) << "" << "}";
    return 0;
}
int TraduitEnCPP::visite(const NoeudInstPour& n) const{
    cout << setw(4*indentation) << "" << "for(";
    Visiteur * v = new TraduitEnCPP(cout, 0, false);
    if(n.getAffect1() != nullptr) n.getAffect1()->applique(v);
    cout << "" << " ; ";
    Visiteur * v2 = new TraduitEnCPP(cout, 0);
    n.getExpression()->applique(v2);
    delete(v2);
    cout << "" << " ; ";
    if(n.getAffect2() != nullptr) n.getAffect2()->applique(v);
    delete(v);
    cout << "" << ") {" << endl;
    Visiteur * v3 = new TraduitEnCPP(cout, indentation+1);
    n.getSequence()->applique(v3);
    delete(v3);
    cout << setw(4*indentation) << "" << "}";
    return 0;
}
int TraduitEnCPP::visite(const NoeudInstSiRiche& n) const{
    cout << setw(4*indentation) << "" << "if(";
    Visiteur * v = new TraduitEnCPP(cout, 0);
    Visiteur * v2 = new TraduitEnCPP(cout, indentation+1);
    n.getExpressions()[0]->applique(v);
    cout << ") {" << endl;
    n.getSequences()[0]->applique(v2);
    cout << setw(4*indentation) << "" << "}";
    for(int i = 1; i<n.getExpressions().size(); i++){
        cout << endl << setw(4*indentation) << "" << "else if(";
        n.getExpressions()[i]->applique(v);
        cout << ") {" << endl;
        n.getSequences()[i]->applique(v2);
        cout << setw(4*indentation) << "" << "}";
    }
    if(n.getSequences().size() > n.getExpressions().size()){
        cout << endl << setw(4*indentation) << "" << "else {" << endl;
        n.getSequences()[n.getSequences().size()-1]->applique(v2);
        cout << setw(4*indentation) << "" << "}";
    }
    delete(v); delete(v2);
    return 0;
}
int TraduitEnCPP::visite(const NoeudInstEcrire& n) const{
    cout << setw(4*indentation) << "" << "cout";
    for(Noeud* noeud : n.getNoeuds()){
        cout << " << ";
        Visiteur * v = new TraduitEnCPP(cout, 0);
        noeud->applique(v);
        delete(v);
    }
    cout << " << endl;";
    return 0;
}
int TraduitEnCPP::visite(const NoeudInstLire& n) const{
    cout << setw(4*indentation) << "" << "cin";
    for(Noeud* variables : n.getVariables()){
        cout << " >> ";
        Visiteur * v = new TraduitEnCPP(cout, 0);
        variables->applique(v);
        delete(v);
        cout << ";";
    }
    return 0;
}
int TraduitEnCPP::visite(const NoeudInstSelon& n) const{
    cout << setw(4*indentation) << "" << "switch(";
    Visiteur * v = new TraduitEnCPP(cout, 0);
    n.getVar()->applique(v);
    delete(v);
    Visiteur * v1 = new TraduitEnCPP(cout, indentation+1);
    Visiteur * v2 = new TraduitEnCPP(cout, indentation+2);
    cout << ") {" << endl;
    for(int i = 0; i < n.getPropales().size(); i++){
        cout << setw(4*(indentation+1)) << "" << "case ";
        n.getPropales()[i]->applique(v1);
        cout << " :" << endl;
        n.getSequences()[i]->applique(v2);
        cout << setw(4*(indentation+2)) << "" << "break;" << endl;
    }
    if(n.getSequences().size() > n.getPropales().size()){
        cout << endl << setw(4*(indentation+1)) << "" << "default :" << endl;
        n.getSequences()[n.getSequences().size()-1]->applique(v2);
    }
    cout << setw(4*indentation) << "" << "}";
    delete(v); delete(v2);
    return 0;
}
int TraduitEnCPP::visite(const SymboleValue& n) const{
    cout << setw(4*indentation) << n.getChaine();
    return 0;
}

TraduitEnCPP::~TraduitEnCPP(){}
