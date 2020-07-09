#ifndef CROSSOVERRESEARCH_OPTIONS_H
#define CROSSOVERRESEARCH_OPTIONS_H

#include <iostream>
#include <ostream>
#include <getopt.h>
#include <string>

struct CLI {
    char *infile;
    unsigned int pop_size;
    unsigned int epochs;
    unsigned int crossover_id;
    double crossover_rate;
    double mutation_rate;
    unsigned int tour_size;
    char *databasefile;
    bool using_db;

    explicit CLI()
    {
        this->infile = (char *) "";
        this->pop_size = 100;
        this->epochs = 50;
        this->crossover_id = 0;
        this->crossover_rate = 0.8;
        this->mutation_rate = 0.05;
        this->tour_size = 8;
        this->databasefile = (char *) "";
        this->using_db = false;
    }

    explicit CLI(char* f, unsigned int p, unsigned int g, unsigned int c, double cr, double mr)
    {
        this->infile = f;
        this->pop_size = p;
        this->epochs = g;
        this->crossover_id = c;
        this->crossover_rate = cr;
        this->mutation_rate = mr;
    }

    friend std::ostream& operator << (std::ostream& os, CLI& cli) {
        std::string cross_name = cli.crossover_id == 0? "Uniforme" : std::to_string(cli.crossover_id).append("-Pontos");
        os << "População  : " << cli.pop_size << "\n";
        os << "N. Gerações: " << cli.epochs << "\n";
        os << "Crossover  : " << cross_name << "\n";
        os << "Tx de cruz.: " << cli.crossover_rate*100 << "%\n";
        os << "Tx de muta.: " << cli.mutation_rate*100 << "%\n";
        os << "Tam.Torneio: " << cli.tour_size << "\n";
        return os;
    }
};

option long_options[] = {
    {"infile", required_argument, 0, 'f'},
    {"db", required_argument, 0, 'd'},
    {"popsize", required_argument, 0, 'p'},
    {"epochs", required_argument, 0, 'g'},
    {"crossover", required_argument, 0, 'x'},
    {"xrate", required_argument, 0, 'c'},
    {"mrate", required_argument, 0, 'm'},
    {"ring", required_argument, 0, 'r'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

const char *short_options = "f:d:p:g:x:c:m:r:h";

const unsigned int NUM_OPTIONS = 9;

const char *DESC[NUM_OPTIONS] = {
    "Arquivo de instância do problema",
    "Arquivo .db para salvar os dados. Se não definido não salva",
    "Tamanho da população [default = 100]",
    "Número de épocas/gerações [default = 100]",
    "Operador de crossover utilizado com um ID [default = 0]",
    "Taxa de cruzamento (%) [default = 0.8]",
    "Taxa de mutação (%) [default = 0.05]",
    "Tamanho do Ring do operador de seleção por torneio determinístico [default = 8]",
    "Mostra essa lista de opções"
};

#endif // CROSSOVERRESEARCH_OPTIONS_H