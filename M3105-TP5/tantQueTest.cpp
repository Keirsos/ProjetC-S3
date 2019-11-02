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

void tantQueTest::testVariables() {
    try{
        
        string nomFich = "testTantQue.txt";
        ifstream fichier(nomFich.c_str());
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        cout << endl << "================ Syntaxe Correcte" << endl;
        
        
        // On affiche le contenu de la table des symboles avant d'exécuter le programme
        cout << endl << "================ Table des symboles apres exécution :" << endl;
        //cout << interpreteur.getTable();
//        cout << endl << "================ Execution de l'arbre" << endl;
//        // On exécute le programme si l'arbre n'est pas vide
//        if (interpreteur.getArbre() != nullptr) interpreteur.getArbre()->executer();
//        // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
//        cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();

        CPPUNIT_ASSERT(true);
    } catch (InterpreteurException & e) {
        CPPUNIT_ASSERT_MESSAGE(e.what(),false);
    }
}

