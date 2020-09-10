#include "set_covering_problem.h"

SetCoveringProblem::SetCoveringProblem(const char *filename) {
  this->__acronym = (char *) "SCP";
  this->__name    = (char *) "Set Covering Problem";
  this->__infilename = (char *) filename;
  this->__minimize = true;

  std::ifstream file(filename);
  if (file.is_open()) {
    this->coverage_matrix = new matrix(file);
    this->__chromSize = this->coverage_matrix->num_columns;
    this->num_subsets = this->coverage_matrix->num_columns;
    this->num_elements = this->coverage_matrix->num_rows;
    this->weights = this->coverage_matrix->get_weights_pointer();

    for (size_t i=0; i < this->coverage_matrix->num_rows; i++) {
      this->all_elements.insert(i);
    }

    // Alimeta os vectores de colunas que cobrem cada linha e linhas cobertas
    // por cada coluna
    this->columns_that_covers = std::vector< std::set<unsigned int> >(this->num_elements);
    this->rows_covered_by = std::vector< std::set<unsigned int> >(this->num_subsets);

    for (size_t row = 0; row < this->num_elements; row++) {
      for (size_t col = 0; col < this->num_subsets; col++) {
        if (this->coverage_matrix->get(row, col)) {
          this->columns_that_covers[row].insert(col);
          this->rows_covered_by[col].insert(row);
        }
      }
    }

  } else {
    std::cerr << "Use a opção -f" << std::endl;
    throw std::runtime_error(
      "Não foi possível abrir o arquivo no caminho: "+ std::string(filename)
    );
  }
}

// Destrutor
SetCoveringProblem::~SetCoveringProblem() {
  delete this->weights;
  delete this->coverage_matrix;
}

// Métodos herdados de "Problem"

matrix* SetCoveringProblem::get_matrix() {
  return this->coverage_matrix;
}

float* SetCoveringProblem::get_weights() {
  return this->coverage_matrix->get_weights_pointer();
}

void SetCoveringProblem::display_info(std::ostream& os) {
  os << this->__name << " (" << this->__acronym << ")\n";
  os << "Número de elementos no conjunto universo: " << this->num_elements << "\n";
  os << "Número de subconjuntos: " << this->num_subsets;
  os << std::endl;
}

/**
 * Versão 1 do método de inicialização de população:
 * Todos os cromossomos da população são inicializados com a mesma taxa de
 * alelos "1", definida pelo param "bias" */
// eoPop<Chrom>
// SetCoveringProblem::init_pop(unsigned int length, double bias) {
//   return Random<Chrom>::population(__chromSize, length, bias);
// }

/**
 * Versão 2 do método de inicialização de população:
 * A população é inicializada com diferentes probabilidades de geração de
 * alelos "1", para cada crossomo é usada uma probabilidade diferente que é
 * gerada aleatoriamente entre o "bias" passado e 0.5 (um valor padrão).
 * OBS: o "bias" não pode ser maior do que 0.5 */
eoPop<Chrom>
SetCoveringProblem::init_pop(unsigned int length, double bias) {
  if (bias > 0.5) {
    std::cerr << __func__ << ": bias não pode ser > 0.5. Corrigindo\n";
    bias = 0.5;
  }

  eoPop<Chrom> aux_pop;
  unsigned int i = 1;
  do {
    Chrom generated;
    eoBooleanGenerator bitRng(Random<Chrom>::uniform(bias, 0.500001));
    eoInitFixedLength<Chrom> init(this->__chromSize, bitRng);
    init(generated);
    aux_pop.push_back(generated);
  } while (i++ < length);

  return aux_pop;
}


std::set<uint> SetCoveringProblem::coverage_set(const Chrom& chrom) {
  // Constrói um set com todas as linhas cobertas pela definição do cromossomo
  std::set<uint> l_rows_covered;
  for (size_t k=0; k < this->__chromSize; k++) {
    if (chrom[k]) {
      l_rows_covered.insert(
        this->rows_covered_by[k].begin(),
        this->rows_covered_by[k].end()
      );
    }
  }
  return l_rows_covered;
}


bool SetCoveringProblem::atend_constraint(const Chrom& chrom) {
  // Conta quantas colunas cobrem cada linha
  // std::vector<int> coverVec(this->num_elements, 0);

  // for (size_t row = 0; row < this->num_elements; row++) {
  //   for (size_t col = 0; col < this->num_subsets; col++) {
  //     if (chrom[col]) {
  //       coverVec[row] += (int) this->coverage_matrix->get(row, col);
  //     }
  //   }
  //   if (coverVec[row] == 0) {
  //     return false;
  //   }
  // }
  std::set<unsigned int> coverage;
  for (size_t i=0; i < chrom.size(); i++) {
    if (chrom[i]) {
      coverage.insert(rows_covered_by[i].begin(), rows_covered_by[i].end());
    }
  }

  return (coverage.size() == this->num_elements);
}


void SetCoveringProblem::make_feasible(Chrom& chrom) {
  if (!this->atend_constraint(chrom)) {
    auto set_rows_covered = this->coverage_set(chrom);
    std::set<uint> diff;
    
    std::set_difference(
      this->all_elements.begin(), this->all_elements.end(), // Copiar todos os elems que não estão
      set_rows_covered.begin(), set_rows_covered.end(), // no conjunto de elementos cobertos
      std::inserter(diff, diff.begin())
    );

    for (auto iter = diff.begin(); iter != diff.end(); ++iter) {
      uint uncovered_row = *iter;
      if (!chrom[*this->columns_that_covers[uncovered_row].begin()]) {
        chrom[*this->columns_that_covers[uncovered_row].begin()] = true;
      }
    }
  }
}


// Função objetivo da cobertura de conjuntos
SetCoveringProblem::Fitness SetCoveringProblem::objective_function(Chrom& chrom)
{
  // Verifica se alguma linha tem contagem = zero
  if (!this->atend_constraint(chrom)) {
    return Fitness(0);
  }

  // Somar os custos das colunas selecionadas
  float accum = 0;
  for (size_t i = 0; i < __chromSize; i++) {
    if (chrom[i]) {
      accum += this->weights[i];
    }
  }

  return (Fitness) (1/accum);
}
