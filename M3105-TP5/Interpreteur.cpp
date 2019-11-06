#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
}

bool contientSymbole(vector<string> v, Symbole s){
    int i = 0;
    while(i < v.size() && !(s == v[i])) i++;
    return i != v.size();
}

bool contientInt(vector<int> v, int l){
    int i = 0;
    while(i < v.size() && l != v[i]) i++;
    return i != v.size();
}

bool Interpreteur::estInstDepart(){
    return (contientSymbole(m_listeInstDepart, m_lecteur.getSymbole()));
}

bool Interpreteur::estInstFin(){
    return (contientSymbole(m_listeInstFin, m_lecteur.getSymbole()));
}

bool Interpreteur::erreurSurLigne(int l){
    return (contientInt(m_lignesErreurs, l));
}

void Interpreteur::erreur(const string & type, const string & str) {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  
  if (type == "Syntaxe"){
      string sym = m_lecteur.getSymbole().getChaine();
      if (sym == "") sym = "<FINDEFICHIER>"; // le symbole de fin de fichier est vide, on le remplace donc pour plus de compréhension
        sprintf(messageWhat,
              "Ligne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
              m_lecteur.getLigne(), str.c_str(), sym.c_str());
    
  } else if (type == "Message") {
        sprintf(messageWhat,
            "Ligne %d - Erreur levée : %s",
            m_lecteur.getLigne(), str.c_str());
        
  }
  
  if (!erreurSurLigne(m_lecteur.getLigne())){
        m_lignesErreurs.push_back(m_lecteur.getLigne());
        cout << messageWhat << endl;
    }
}

bool Interpreteur::testerBool(const string & symboleAttendu) const {
  // Teste si le symbole courant est égal au symboleAttendu... en booléen
  return (m_lecteur.getSymbole() == symboleAttendu);
}

bool Interpreteur::tester(const string & symboleAttendu) {
    // Teste si le symbole courant est égal au symboleAttendu...
    
    if (m_lecteur.getSymbole() != symboleAttendu) {
        erreur("Syntaxe", symboleAttendu);
        return false;
    }
    return true;
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance
    if (tester(symboleAttendu)) m_lecteur.avancer();
}

Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqProgramme();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  
  if (!m_lignesErreurs.empty()){
    static char messageErreur[256];
    sprintf(messageErreur, "%ld erreurs trouvées, arrêt du programme", m_lignesErreurs.size());
    throw SyntaxeException(messageErreur);
  }
  
  return sequence;
}

Noeud* Interpreteur::seqProgramme() {
    NoeudSeqInst* sequence = new NoeudSeqInst();
    
    do {
        
        sequence->ajoute(inst());
        
        if (m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "<FINDEFICHIER>"){ // même condition que le while
            if (!estInstDepart()){
                erreur("Message", m_lecteur.getSymbole().getChaine());
                while (!estInstFin()){
                    m_lecteur.avancer();
                }
            }
            if (!estInstDepart()){ // On rentre dans cette boucle après le traitement du if précédent, donc ne surtout pas combiner les deux if
                while (!estInstDepart() && m_lecteur.getSymbole() != "finproc"){
                    m_lecteur.avancer();
                }
            }
        }
        
    } while(m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "<FINDEFICHIER>"); // Tant que le programme n'es pas fini
    
    return sequence;
}

Noeud* Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    
    NoeudSeqInst* sequence = new NoeudSeqInst();
    
    do {
        sequence->ajoute(inst());
    } while(estInstDepart()); // Tant que le symbole courant est un début possible d'instruction
    
    return sequence;
}

Noeud* Interpreteur::inst() {
  // <inst> ::= <affectation>  ; | <instSi>
    if (m_lecteur.getSymbole() == ";"){
        m_lecteur.avancer();
        return nullptr;
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
    else if (m_lecteur.getSymbole() == "selon")
        return instSelon();
    // ajouter ici les nouvelles instructions
    else if (m_lecteur.getSymbole() == "<VARIABLE>") {
        // à ne pas modifier, c'est normal que ce soit différent
        Noeud *affect = affectation();
        testerEtAvancer(";");
        return affect;
    }
    else {
        erreur("Message", "Première instruction de la séquence incorrecte");
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
    Noeud* fact = expEt();
    while(m_lecteur.getSymbole() == "ou"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* factDroit = expEt();
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit);
    }
    return fact;
}

Noeud* Interpreteur::expEt() {
    Noeud* fact = expComp();
    while(m_lecteur.getSymbole() == "et"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* factDroit = expComp();
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit);
    }
    return fact;
}

Noeud* Interpreteur::expComp() {
    Noeud* fact = expAdd();
    while(m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
          m_lecteur.getSymbole() == "<" || m_lecteur.getSymbole() == "<="  ||
          m_lecteur.getSymbole() == ">" || m_lecteur.getSymbole() == ">="){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* factDroit = expAdd();
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit);
    }
    return fact;
}

Noeud* Interpreteur::expAdd() {
    Noeud* fact = expMult();
    while(m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* factDroit = expMult();
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit);
    }
    return fact;
}

Noeud* Interpreteur::expMult() {
    Noeud* fact = facteur();
    while(m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* factDroit = facteur();
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit);
    }
    return fact;
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
    // inversion binaire
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else {
      erreur("Message", "Facteur incorrect");
  }
  
  return fact;
}

Noeud* Interpreteur::instTantQue() {
    // <instTantQue> ::=tantque( <expression> ) <seqInst> fintantque
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);
}

Noeud* Interpreteur::instRepeter() {
    // <instRepeter> ::=repeter <seqInst> jusqua( <expression> )
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
    // <instPour>    ::=pour( [ <affectation> ] ; <expression> ;[ <affectation> ]) <seqInst> finpour
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
    // <instSiRiche> ::=si(<expression>) <seqInst> {sinonsi(<expression>) <seqInst> }[sinon <seqInst>]finsi
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
    // <instEcrire>  ::=ecrire( <expression> | <chaine> {, <expression> | <chaine> })
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
    // <instLire>    ::=lire( <variable> {, <variable> })   
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

Noeud* Interpreteur::instSelon() {
    // <instSelon> ::= selon (<variable>) {cas <entier> : <seqInst>} [defaut : <seqInst>] finselon
    Noeud* var;
    vector<Noeud*> propales;
    vector<Noeud*> sequences;
    
    testerEtAvancer("selon");
    testerEtAvancer("(");
    tester("<VARIABLE>");
    var = new SymboleValue(m_lecteur.getSymbole());
    m_lecteur.avancer();
    testerEtAvancer(")");
    while(!testerBool("defaut") && !testerBool("finselon")){
        testerEtAvancer("cas");
        tester("<ENTIER>");
        propales.push_back(new SymboleValue(m_lecteur.getSymbole()));
        m_lecteur.avancer();
        testerEtAvancer(":");
        sequences.push_back(seqInst());
    }
    if(testerBool("defaut")){
        testerEtAvancer("defaut");
        testerEtAvancer(":");
        sequences.push_back(seqInst());
    }
    testerEtAvancer("finselon");
    
    //return nullptr;
    return new NoeudInstSelon(var, propales, sequences);
}

void Interpreteur::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation) << "" << "#include <iostream>" << endl << "using namespace std;" << endl << endl;
    cout << setw(4*indentation) << "" << "int main() {" << endl; //Début d'un programme C++
    
    for(int i = 0; i<m_table.getTaille(); i++){
        if(!m_table[i].isEntier()){
            cout << setw(4*(indentation+1)) << "" << "int ";
//            Visiteur * v = new TraduitEnCPP(cout, 0);
//            m_table[i].applique(v);
//            delete(v);
            m_table[i].traduitEnCPP(cout, 0);
            cout << ";" << endl;
        }
    }
    cout << endl;
    
    getArbre()->traduitEnCPP(cout,indentation+1); //Lance l'opération traduitEnCPP sur la racine
    cout << setw(4*(indentation+1)) << "" << "return 0;" << endl;
    cout << setw(4*indentation) << "}" << endl; //Fin d'un programme C++
}
