/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   tantQueTest.cpp
 * Author: Flavien
 *
 * Created on 2 nov. 2019, 16:25:03
 */

#include "tantQueTest.h"

//

#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"


CPPUNIT_TEST_SUITE_REGISTRATION(tantQueTest);

tantQueTest::tantQueTest() {
}

tantQueTest::~tantQueTest() {
}

void tantQueTest::setUp() {
}

void tantQueTest::tearDown() {
}

void tantQueTest::testMethod() {
    CPPUNIT_ASSERT(true);
}

void tantQueTest::testFailedMethod() {
    CPPUNIT_ASSERT(false);
}

/*****************************************************************/

void tantQueTest::testSyntaxe() {
    try{
        
        string nomFich = "testTantQue.txt";
        ifstream fichier(nomFich.c_str());
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        
        CPPUNIT_ASSERT(true);
    } catch (InterpreteurException & e) {
        CPPUNIT_ASSERT_MESSAGE(e.what(),false);
    }
}

void tantQueTest::testNombreErreurs() {
    string nomFich = "testTantQueErreurs.txt"; // 3 erreurs volontaires
    ifstream fichier(nomFich.c_str());
    Interpreteur interpreteur(fichier);
    
    try{
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi (ce n'est pas ce que l'on veut)
        
        CPPUNIT_ASSERT(false);
    } catch (InterpreteurException & e) {
        CPPUNIT_ASSERT_EQUAL(3, interpreteur.getNbErreurs());
    }
}

void tantQueTest::testVariables() {
    try{
        
        string nomFich = "testTantQue.txt";
        ifstream fichier(nomFich.c_str());
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        
        // On exécute le programme si l'arbre n'est pas vide
        Visiteur * v = new Executer();
        if (interpreteur.getArbre() != nullptr){
            interpreteur.getArbre()->applique(v);
            delete(v);
        }
        
        const Symbole* i = new Symbole("i");
        
        CPPUNIT_ASSERT_EQUAL(10, (interpreteur.getTable().chercheAjoute(*i))->applique(v)); // i est censé etre égal à 10, on le vérifie
    } catch (InterpreteurException & e) {
        CPPUNIT_ASSERT_MESSAGE(e.what(),false);
    }
}

