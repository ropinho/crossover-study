#include <iostream>
#include <string>
#include <paradiseo/eo/ga/eoBitOp.h>
#include <paradiseo/eo/eoGenContinue.h>
#include <paradiseo/eo/eoTimeContinue.h>
#include <paradiseo/eo/eoRankingSelect.h>
// #include <paradiseo/eo/eoDetTournamentSelect.h>

#include <core/cli/parse.h>
#include <core/ga/encoding.h>
#include <core/ga/random.h>
#include <core/ga/crossover_fabric.h>
#include <core/ga/genetic_algorithm.h>
#include <core/utils/split.h>
#include <core/time_parse.hpp>
#include <core/db/create.hpp>
#include <core/db/entry.hpp>

#include "maximum_clique_problem.h"

#define print(txt) std::cout << txt << std::flush
#define println(txt) std::cout << txt << std::endl
#define sepline(n) \
  for (int i=0; i < n; i++) { \
    std::cout << '-'; \
  } \
  std::cout << std::endl

// Unix Escape Color
#define UEC(number) "\e[38;5;"+ std::to_string(number) +"m"

using Str = std::string;


/**
 * Função executada dentro do algortimo genético a cada geração após os
 * a aplicação dos operadores genéticos
 */
void __within_generations (int g, eoPop<Chrom> &p) {
#ifdef __unix__
  std::cout << UEC(86) << std::flush;
#endif
  std::cout << "G" << g << " -- Melhor fitness: " << p.best_element().fitness();
#ifdef __unix__
  std::cout << UEC(255) << std::endl;
#endif
}


int exec(int argc, char **argv) {
  using namespace std::chrono;
  std::cout << "\e[1;3m\e[38;5;47mProblema do Clique Máximo\e[0m" << std::endl;

  auto args = parse(argc, argv);
  auto filename = *(
    split(Str(args->infile), '/').end()-1
  );

  // leitura do arquivo de definição do grafo e instanciação do problema
  MCProblem mc(args->infile);
  println("Matrix inicializada: arquivo " << filename);

  sepline(60);
  std::cout << *args;
  sepline(60);

  // Geração da população inicial
  auto pop = mc.init_pop(args->pop_size, 0.25);
  println("População inicializada");

  // Avaliando população inicial
  sepline(60);
  print("Avaliando população inicial");
  mc.eval(pop);
  println("\rPopulação inicial avaliada ");

  // Definição dos parâmetros do AG
  eoGenContinue<Chrom> term(args->epochs);
  eoBitMutation<Chrom> mutation( args->mutation_rate );
  eoRankingSelect<Chrom> select;
  auto *crossover = CrossoverFabric::create(args->crossover_id);

  // define a instância da classe de Algoritmo genético
  GeneticAlgorithm ga(
    mc, select, *crossover, args->crossover_rate, mutation, 1.0f, term);
  sepline(60);

  std::cout << UEC(47) << "Iniciando evolução" << UEC(255) << std::endl;

  // Vector de convergências
  std::vector<Chrom> conv;

  // Execução da evolução
  auto start_point = std::chrono::system_clock::now();
  ga(pop, conv, __within_generations);
  auto dur_ns = system_clock::now() - start_point; // Duração em nanosegundos

  sepline(60);

  // Converter duração para segundos
  std::cout << std::fixed << "Tempo de evolução: " << UEC(47)
            << parse_duration(dur_ns) << UEC(255) << std::endl;

  // Solução final
  sepline(60);
  Chrom melhor = pop.best_element();
  std::vector<int> solution;

  println("Melhor solução:");
  println("Clique de " << (uint) melhor.fitness() << " vértices");
  print("Vértices: ");

  // Iterações pelo cromossomo pegando os índices dos genes '1' para definir
  // os vértices que fazem parte da solução final encontrada
  for (size_t i = 0; i < melhor.size(); i++) {
    if (melhor[i]) {
      solution.push_back(i+1);
      print(i+1 << " ");
    }
  }
  std::cout << std::endl;

  // Escrevenco dados no banco se for definido um argumento para
  // a opção "--db"
  if (args->using_db) {
    int clqsize = (int) melhor.fitness();

    auto con = db_create(args->databasefile, db_structure::mc);
    db_entry(*con, db_structure::mc, args, clqsize, solution, conv,
        filename, duration_cast<milliseconds>(dur_ns));

    std::cout << "Dados escritos em " << args->databasefile << std::endl;
  }

  return 0;
}


int main(int argc, char **argv) {
  try {
    return exec(argc, argv);
  } catch (std::exception &e) {
    std::cerr << "Exception catched: " << e.what() << std::endl;
    return 127;
  }
  return 0;
}