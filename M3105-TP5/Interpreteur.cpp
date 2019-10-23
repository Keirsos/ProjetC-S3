#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr), m_nbErreurs(0) {
}

void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
}

bool contient(vector<string> v, Symbole s){
    int i = 0;
    while(i < v.size() && !(s == v[i])) i++;
    return i != v.size();
}

bool Interpreteur::estInstDepart(){
    return (contient(m_listeInstDepart, m_lecteur.getSymbole()));
}

bool Interpreteur::estInstFin(){
    return (contient(m_listeInstFin, m_lecteur.getSymbole()));
}

bool Interpreteur::testerBool(const string & symboleAttendu) const {
  // Teste si le symbole courant est égal au symboleAttendu... en booléen
  return (m_lecteur.getSymbole() == symboleAttendu);
}

bool Interpreteur::tester(const string & symboleAttendu) {
    // Teste si le symbole courant est égal au symboleAttendu...
    static char messageWhat[256];
    if (m_lecteur.getSymbole() != symboleAttendu) {
        sprintf(messageWhat,
                "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
                m_lecteur.getLigne(), m_lecteur.getColonne(),
                symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
        cout << messageWhat << endl;
        incrErreurs();
        return false;
    }
    return true;
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance
    if (tester(symboleAttendu)) m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat,
          "Ligne %d, Colonne %d - Erreur levée : %s",
          m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str());
    cout << messageWhat << endl;
    incrErreurs();
}

Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  
  if (m_nbErreurs != 0){
    static char messageErreur[256];
    sprintf(messageErreur, "%d erreurs trouvées, arrêt du programme", m_nbErreurs);
    throw SyntaxeException(messageErreur);
  }
  
  return sequence;
}

Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
//      if (m_lecteur.getSymbole() == ";"){
//        cout << "test" << endl;
//        m_lecteur.avancer();
//      }
//      else 
      if (estInstFin()){
        while (!estInstDepart()){
            erreur(m_lecteur.getSymbole().getChaine());
            m_lecteur.avancer();
        }
      }
        
    sequence->ajoute(inst());
  } while(estInstDepart());
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() {
  // <inst> ::= <affectation>  ; | <instSi>
  if (m_lecteur.getSymbole() == "<VARIABLE>") {
    Noeud *affect = affectation();
    testerEtAvancer(";");
    return affect;
  }
  else if (m_lecteur.getSymbole() == "si")
      return instSiRiche();
  else if (m_lecteur.getSymbole() == "tantque")
      return instTantQue();
  else if (m_lecteur.getSymbole() == "repeter")
      return instRepeter();
  else if (m_lecteur.getSymbole() == "pour")
      return instPour();
  else if (m_lecteur.getSymbole() == "ecrire")
      return instEcrire();
  else if (m_lecteur.getSymbole() == "lire")
      return instLire();
  // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
  else {
      erreur("Instruction incorrecte");
      return nullptr;
  }
}

Noeud* Interpreteur::affectation() {
  // <affectation> ::= <variable> = <expression> 
  tester("<VARIABLE>");
  Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
  m_lecteur.avancer();
  testerEtAvancer("=");
  Noeud* exp = expression();             // On mémorise l'expression trouvée
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
  // <expression> ::= <facteur> { <opBinaire> <facteur> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
  Noeud* fact = facteur();
  while ( m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-"  ||
          m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/"  ||
          m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
          m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
          m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
          m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou"   ) {
    Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
    m_lecteur.avancer();
    Noeud* factDroit = facteur(); // On mémorise l'opérande droit
    fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construit un noeud opérateur binaire
  }
  return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
  Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
    fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else
    erreur("Facteur incorrect");
  return fact;
}

Noeud* Interpreteur::instTantQue() {
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);
}

Noeud* Interpreteur::instRepeter() {
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstRepeter(sequence, condition);
}

Noeud* Interpreteur::instPour(){
    Noeud* affect1 = nullptr;
    Noeud* affect2 = nullptr;
    
    testerEtAvancer("pour");
    testerEtAvancer("(");
    if (testerBool("<VARIABLE>")){
        affect1 = affectation();
    }
    testerEtAvancer(";");
    Noeud* expre = expression();
    testerEtAvancer(";");
    if (testerBool("<VARIABLE>")){
        affect2 = affectation();
    }
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finpour");
    
    return new NoeudInstPour(affect1, expre, affect2, sequence);
}

Noeud* Interpreteur::instSiRiche(){
    vector<Noeud*> expressions;
    vector<Noeud*> sequences;
    
    testerEtAvancer("si");
    testerEtAvancer("(");
    expressions.push_back(expression());
    testerEtAvancer(")");
    sequences.push_back(seqInst());
    while(testerBool("sinonsi")){
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        expressions.push_back(expression());
        testerEtAvancer(")");
        sequences.push_back(seqInst());
    }
    if(testerBool("sinon")){
        testerEtAvancer("sinon");
        sequences.push_back(seqInst());
    }
    testerEtAvancer("finsi");
    
    return new NoeudInstSiRiche(expressions, sequences);
}

Noeud* Interpreteur::instEcrire(){
    
    vector<Noeud*> noeuds;
    
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    
    if(testerBool(",")) tester(",");
    
    while(!testerBool(")")){
        if(testerBool(",")) m_lecteur.avancer();
        if(testerBool("<CHAINE>")){
            SymboleValue* chaine = new SymboleValue(m_lecteur.getSymbole());
            noeuds.push_back(chaine);
            m_lecteur.avancer();
        }
        else{
            noeuds.push_back(expression());
        }
    }
    
    testerEtAvancer(")");
    testerEtAvancer(";");
    
    return new NoeudInstEcrire(noeuds);
}

Noeud* Interpreteur::instLire() {
    vector<Noeud*> variables;
    
    testerEtAvancer("lire");
    testerEtAvancer("(");
    tester("<VARIABLE>");
    
    while (!testerBool(")")){
        
        if(testerBool(",")) m_lecteur.avancer();
        
        tester("<VARIABLE>");
        Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole());
        variables.push_back(var);
        m_lecteur.avancer();
    }
    
    testerEtAvancer(")");
    testerEtAvancer(";");
    
    return new NoeudInstLire(variables);
}