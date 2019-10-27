#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"

int main(int argc, char* argv[]) {
    string nomFich, nomFichTrad;
    if (argc < 2) {
        cout << "Usage : " << argv[0] << " nom_fichier_source nom_fichier_traduction" << endl << endl;
        cout << "Entrez le nom du fichier que vous voulez interpréter : ";
        getline(cin, nomFich);
        cout << "Entrez le nom du fichier dans lequel vous voulez écrire la traduction : ";
        getline(cin, nomFichTrad);
    } else{
        nomFich = argv[1];
        nomFichTrad = argv[2];
    }
    ifstream fichier(nomFich.c_str());
    ofstream fichierTrad(nomFichTrad.c_str());
    try {
        if (fichier.fail()) throw FichierException();
        if (fichierTrad.fail()) throw FichierException();
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        cout << endl << "================ Syntaxe Correcte" << endl;
        // On affiche le contenu de la table des symboles avant d'exécuter le programme
        cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
        cout << endl << "================ Execution de l'arbre" << endl;
        // On exécute le programme si l'arbre n'est pas vide
        if (interpreteur.getArbre() != nullptr) interpreteur.getArbre()->executer();
        // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
        cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();
        
        //Ecriture dans le fichier de traduction
        interpreteur.traduitEnCPP(fichierTrad,0);
    } catch (InterpreteurException & e) {
        cout << e.what() << endl;
    }
    return 0;
}
