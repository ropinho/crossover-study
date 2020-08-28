#include <iostream>
#include <iomanip>
#include <chrono>
using namespace std::chrono;

#include <paradiseo/eo/ga/eoBitOp.h>
#include <paradiseo/eo/eoGenContinue.h>
#include <paradiseo/eo/eoTimeContinue.h>
#include <paradiseo/eo/eoRankingSelect.h>
#include <sqlite/database_exception.hpp>
#include <core/cli/parse.h>
#include <core/ga/genetic_algorithm.h>
#include <core/ga/crossover_fabric.h>
#include <core/db/create.hpp>
#include <core/db/entry.hpp>
#include <core/time_parse.hpp>

#include "scp_matrix.h"
#include "set_covering_problem.h"

#define green(msg) "\e[1;32m" + std::string(msg) + "\e[0m"

/**
 * Printa vários "-" para separar a linha anterior da próxima no log.
 * O número padrão de "-"s é 60 */
inline void break_lines(std::ostream &os, unsigned short __n = 60) {
  unsigned short n = 0;
  while (n++ < __n)
    os << '-';
  os << std::endl;
}

/**
 * Corta o caminho do arquivo reduzindo-o a somente o
 * nome do arquivo */
const char* trim_filename(const char *filename) {
  std::string str_filename(filename);
  return (split(str_filename, '/').end()-1)->c_str();
}

/**
 * Funçção de callback entre as geração do algoritmo genético */
void ga_callback(int generation, eoPop<Chrom>& pop) {
  std::cout << "Geração " << generation << ": ";
  std::cout << (int) (1 / pop.it_best_element()->fitness()) << std::endl;
}

/**
 * Função de execução d programa principal inteiro
 */
int exec(CLI *args) {
  const char *fname = trim_filename(args->infile);
  std::cout << "Cobertura de conjuntos: " << fname << std::endl;
  break_lines(std::cout);

  SetCoveringProblem prob(args->infile);
  prob.display_info(std::cout);
  break_lines(std::cout);

  std::cout << *args;
  break_lines(std::cout);

  std::cout << "Inicializando população " << std::flush;
  auto pop = prob.init_pop( args->pop_size );
  std::cout << "[" << green("done") << "]" << std::endl;

  std::cout << "Avaliando população inicial " << std::flush;
  prob.eval(pop);

  // Definição do Algoritmo genético e operadores
  eoGenContinue<Chrom> term(args->epochs);
  eoBitMutation<Chrom> mutation( args->mutation_rate );
  eoRankingSelect<Chrom> select;
  auto *crossover = CrossoverFabric::create(args->crossover_id);
  GeneticAlgorithm ga(
    prob, select, *crossover, args->crossover_rate, mutation, 1.0f, term);

  // Início da execução do AG
  break_lines(std::cout);
  std::cout << green("Iniciando evolução") << std::endl;
  std::vector<Chrom> convergence;

  // auto start_point = system_clock::now();
  ga(pop, convergence, ga_callback);
  // auto end_point   = system_clock::now();

  return 0;
}


/**
 * Função principal:
 * Encapsula a execução do programa todo com um bloco try/catch para
 * lidar com possíveis exceções não tratadas
 */
int main(int argc, char **argv) {
  try {
    CLI* arguments = parse(argc, argv);
    return exec(arguments);
  }
  catch (std::exception& e) {
    std::cerr << "Exception at main(): \"" << e.what() << "\"" << std::endl;
    return 1;
  }
  return 0;
}