#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

//#include <algorithm>
#include <iterator>

#include "Symbole.h"
#include "Lecteur.h"
#include "Exceptions.h"
#include "TableSymboles.h"
#include "ArbreAbstrait.h"

class Interpreteur {
public:
	Interpreteur(ifstream & fichier);   // Construit un interpréteur pour interpreter
	                                    //  le programme dans  fichier 
                                      
	void analyse();                     // Si le contenu du fichier est conforme à la grammaire,
	                                    //   cette méthode se termine normalement et affiche un message "Syntaxe correcte".
                                      //   la table des symboles (ts) et l'arbre abstrait (arbre) auront été construits
	                                    // Sinon, une exception sera levée

	inline const TableSymboles & getTable () const  { return m_table;    } // accesseur	
	inline Noeud* getArbre () const { return m_arbre; }                    // accesseur
	
private:
    Lecteur        m_lecteur;  // Le lecteur de symboles utilisé pour analyser le fichier
    TableSymboles  m_table;    // La table des symboles valués
    Noeud*         m_arbre;    // L'arbre abstrait
    
    const vector<string> m_listeInstDepart = {";","<VARIABLE>","si","tantque","repeter","pour","ecrire","lire"};
    const vector<string> m_listeInstFin = {";","finsi","fintantque","finpour","finproc","<FINDEFICHIER>"};
    vector<int> m_lignesErreurs;
    
    // Implémentation de la grammaire
    Noeud*  programme();   //   <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    Noeud*  inst();	       //        <inst> ::= <affectation> ; | <instSi>
    Noeud*  affectation(); // <affectation> ::= <variable> = <expression> 
    Noeud*  expression();  //  <expression> ::= <facteur> { <opBinaire> <facteur> }
    Noeud*  facteur();     //     <facteur> ::= <entier>  |  <variable>  |  - <facteur>  | non <facteur> | ( <expression> )
                           //   <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    
    Noeud* seqProgramme();
    Noeud* seqInst();	   //     <seqInst> ::= <inst> { <inst> }
    Noeud* instTantQue();
    Noeud* instRepeter();
    Noeud* instPour();
    Noeud* instSiRiche();
    Noeud* instEcrire();
    Noeud* instLire();

    // outils pour simplifier l'analyse syntaxique
    bool tester(const string & symboleAttendu);
    void testerEtAvancer(const string & symboleAttendu); // Si symbole courant != symboleAttendu, on lève une exception, sinon on avance
    void erreur (const string & type, const string & str = "");             // Lève une exception "contenant" le message mess
    bool erreurSurLigne (int l);
    
    bool testerBool(const string & symboleAttendu) const;
    bool estInstDepart();
    bool estInstFin();
};

#endif /* INTERPRETEUR_H */