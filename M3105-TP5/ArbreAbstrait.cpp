#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    for(Noeud* noeud : m_instructions){
        noeud->traduitEnCPP(cout,indentation);
        cout << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    cout << setw(4*indentation) << "";
    m_variable->traduitEnCPP(cout,0);
    cout << " = " << "";
    m_expression->traduitEnCPP(cout,0);
    if(pointVirgule) cout << ";";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    cout << setw(4*indentation) << "";
    cout << "(";
    m_operandeGauche->traduitEnCPP(cout,0);
    string operateur = m_operateur.getChaine();
    if (operateur == "et")      operateur = "&&";
    else if (operateur == "ou") operateur = "||";
    cout << " " << operateur << " ";
    m_operandeDroit->traduitEnCPP(cout,0);
    cout << ")";
}

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
  while (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    cout << setw(4*indentation) << "" << "while(";
    m_condition->traduitEnCPP(cout,0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+1);
    cout << setw(4*indentation) << "" << "}";
}

NoeudInstRepeter::NoeudInstRepeter(Noeud* sequence, Noeud* condition)
: m_sequence(sequence), m_condition(condition) {
}

int NoeudInstRepeter::executer() {
    if(!m_condition->executer()){
        do
            m_sequence->executer();
        while(!m_condition->executer());
    }
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstRepeter::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    cout << setw(4*indentation) << "" << "if(!(";
    m_condition->traduitEnCPP(cout,0);
    cout << ")) {" << endl << setw(4*(indentation+1)) << "" << "do" << endl;
    m_sequence->traduitEnCPP(cout,indentation+2);
    cout << setw(4*(indentation+1)) << "" << "while(!(";
    m_condition->traduitEnCPP(cout,0);
    cout << "));" << endl << setw(4*indentation) << "" << "}";
}

NoeudInstPour::NoeudInstPour(Noeud* affectation1, Noeud* expression, Noeud* affectation2, Noeud* sequence)
: m_affectation1(affectation1), m_expression(expression), m_affectation2(affectation2), m_sequence(sequence) {
}

int NoeudInstPour::executer() {
    
    if(m_affectation1 != nullptr) m_affectation1->executer();
    while(m_expression->executer()){
        m_sequence->executer();
        if(m_affectation2 != nullptr) m_affectation2->executer();
    }
    
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstPour::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    cout << setw(4*indentation) << "" << "for(";
    if(m_affectation1 != nullptr) m_affectation1->traduitEnCPP(cout,0,false);
    cout << "" << " ; ";
    m_expression->traduitEnCPP(cout,0);
    cout << "" << " ; ";
    if(m_affectation2 != nullptr) m_affectation2->traduitEnCPP(cout,0,false);
    cout << "" << ") {" << endl;
    m_sequence->traduitEnCPP(cout,indentation+1);
    cout << setw(4*indentation) << "" << "}";
}

NoeudInstSiRiche::NoeudInstSiRiche(std::vector<Noeud*> expressions, std::vector<Noeud*> sequences)
: m_expressions(expressions), m_sequences(sequences){
}

int NoeudInstSiRiche::executer(){
    
    int i = 0;
    while(i < m_expressions.size() && !(m_expressions.at(i)->executer())){
        i++;
    }
    if (i < m_expressions.size()) m_sequences.at(i)->executer();
    else                          m_sequences.at(m_sequences.size()-1)->executer();
    
    return 0;
}

void NoeudInstSiRiche::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    cout << setw(4*indentation) << "" << "if(";
    m_expressions[0]->traduitEnCPP(cout,0);
    cout << ") {" << endl;
    m_sequences[0]->traduitEnCPP(cout,indentation+1);
    cout << setw(4*indentation) << "" << "}";
    for(int i = 1; i<m_expressions.size(); i++){
        cout << endl << setw(4*indentation) << "" << "else if(";
        m_expressions[i]->traduitEnCPP(cout,0);
        cout << ") {" << endl;
        m_sequences[i]->traduitEnCPP(cout, indentation+1);
        cout << setw(4*indentation) << "" << "}";
    }
    if(m_sequences.size() > m_expressions.size()){
        cout << endl << setw(4*indentation) << "" << "else {" << endl;
        m_sequences[m_sequences.size()-1]->traduitEnCPP(cout, indentation+1);
        cout << setw(4*indentation) << "" << "}";
    }
}

NoeudInstEcrire::NoeudInstEcrire(vector<Noeud*> noeuds)
: m_noeuds(noeuds) {
}

int NoeudInstEcrire::executer() {
    
    for(Noeud* noeud : m_noeuds){
        if((typeid(*noeud)==typeid(SymboleValue) &&  *((SymboleValue*)noeud)== "<CHAINE>" )){
            string chaine = ((SymboleValue*)noeud)->getChaine();
            chaine.erase(0,1); chaine.erase(chaine.size()-1,1);
            cout << chaine;
        }
        else{
            cout << noeud->executer();
        }
    }
    cout << endl;
    
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstEcrire::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    cout << setw(4*indentation) << "" << "cout";
    for(Noeud* noeud : m_noeuds){
        cout << " << ";
        noeud->traduitEnCPP(cout,0);
    }
    cout << " << endl;";
}


NoeudInstLire::NoeudInstLire(vector<Noeud*> variables)
: m_variables(variables) {
}

int NoeudInstLire::executer() {
    
    for(Noeud* var : m_variables){
        int temp;
        cin >> temp;
        ((SymboleValue*)var)->setValeur(temp);
    }
    
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstLire::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    cout << setw(4*indentation) << "" << "cin";
    for(Noeud* variables : m_variables){
        cout << " >> ";
        variables->traduitEnCPP(cout,0);
        cout << ";";
    }
}
NoeudInstSelon::NoeudInstSelon(Noeud* var, vector<Noeud*> propales, vector<Noeud*> sequences) :
    m_var(var),
    m_propales(propales),
    m_sequences(sequences)
{}

int NoeudInstSelon::executer() {
    int i = 0;
    while(i < m_propales.size() && m_var != m_propales[i]){
        i++;
    }
    if (i < m_propales.size())  m_sequences[i]->executer();
    else                        m_sequences[m_sequences.size()-1]->executer();
    
    return 0;
}

void NoeudInstSelon::traduitEnCPP(ostream& cout, unsigned int indentation, bool pointVirgule) const{
    cout << setw(4*indentation) << "" << "switch(";
    m_var->traduitEnCPP(cout,0);
    cout << ") {" << endl;
    for(int i = 0; i < m_propales.size(); i++){
        cout << setw(4*(indentation+1)) << "" << "case ";
        m_propales[i]->traduitEnCPP(cout,indentation+1);
        cout << " :" << endl;
        m_sequences[i]->traduitEnCPP(cout, indentation+2);
        cout << setw(4*(indentation+2)) << "" << "break;" << endl;
    }
    if(m_sequences.size() > m_propales.size()){
        cout << endl << setw(4*(indentation+1)) << "" << "default :" << endl;
        m_sequences[m_sequences.size()-1]->traduitEnCPP(cout, indentation+2);
    }
    cout << setw(4*indentation) << "" << "}";
}