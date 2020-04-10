//
// Created by ronaldd on 09/04/2020.
//

#include "configuration.h"
#include "config_error.h"

const std::vector<std::string> REQUIRED_LABELS = {"id", "population",
		"generations", "crossover_rate", "mutation_rate", "crossover"};


Configuration::Configuration(std::string filename)
: config(YAML::LoadFile(filename)) {
	if (!config.IsMap())
		throw ConfigError("Config file should be a map");
	for (auto &lbl : REQUIRED_LABELS) {
		if (!config[lbl].IsDefined()) {
			throw ConfigError(filename+": Parâmetro \""+lbl+"\" não definido!");
		}
		if (!config[lbl].IsScalar()) {
			throw ConfigError(filename+": \""+lbl+"\" deve ser um escalar!");
		}
	}
}

std::string Configuration::ID() {
	return (std::string) this->config["id"].as<std::string>();
}

unsigned int Configuration::popsize() {
	return (uint) this->config["population"].as<uint>();
}

unsigned int Configuration::number_generations() {
	return (uint) this->config["generations"].as<uint>();
}

std::string Configuration::crossover_name() {
	return (std::string) this->config["crossover"].as<std::string>();
}

double Configuration::crossover_rate() {
	return (double) this->config["crossover_rate"].as<double>();
}

double Configuration::mutation_rate() {
	return (double) this->config["mutation_rate"].as<double>();
}

std::ostream & Configuration::operator<<(std::ostream &os) {
	os << "Config: " << this->ID() << "\n";
	os << " Op de crossover: " << this->crossover_name() << "\n";
	os << " Taxa de cruzamento: " << this->crossover_rate() << "\n";
	os << " Taxa de mutação: " << this->mutation_rate() << "\n";
	os << " Tam. População: " << this->popsize() << "\n";
	os << " Num. de gerações: " << this->number_generations() << "\n";
	return os;
}
