#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <memory>
#include <cstring>


class OandD {
private:
    char* origine;
    char* destination;
    std::set<long> tarifs;

public:
    OandD(const char* orig, const char* dest, const std::set<long>& tar) : origine(strdup(orig)), destination(strdup(dest)), tarifs(tar) {}
    ~OandD() {
        free(origine);
        free(destination);
    }

    const char* getOrigine() const { return origine; }
    const char* getDestination() const { return destination; }
    std::set<long> getTarifs() const { return tarifs; }
    void setTarifs(const std::set<long>& tar) { tarifs = tar; }

    long minTarif() const { return *tarifs.begin(); }
    long maxTarif() const { return *tarifs.rbegin(); }
    double moyenneTarif() const {
        if (tarifs.empty()) return 0.0;
        
        double sum = std::accumulate(tarifs.begin(), tarifs.end(), 0.0,
            [](double acc, long tarif) { return acc + static_cast<double>(tarif) / 100.0; });

        double average = sum / static_cast<double>(tarifs.size());
        return average;
    }


    void addTarif(long tarif) {
        tarifs.insert(tarif);
    }

    std::string exportData() const {
        std::stringstream ss;
        ss << origine << "," << destination << ","
           << std::fixed << std::setprecision(2) << static_cast<double>(minTarif()) / 100.0 << ","
           << std::fixed << std::setprecision(2) << static_cast<double>(maxTarif()) / 100.0 << ","
           << std::fixed << std::setprecision(2) << moyenneTarif();
        return ss.str();
    }
    
    static void writeColumnNames(const std::string& fichier) {
            std::ofstream outFile(fichier, std::ofstream::out);
            if (outFile.is_open()) {
                outFile << "Origine,Destination,TarifMinimum,TarifMaximum,TarifMoyen\n";
            } else {
                std::cerr << "Impossible d'ouvrir le fichier: " << fichier << std::endl;
            }
        }

};

class FlightNbr {
private:
    char* numeroVol;
    OandD* od;

public:
    FlightNbr(const char* numVol, OandD* odObj) : numeroVol(strdup(numVol)), od(odObj) {}

    ~FlightNbr() {
        free(numeroVol);
    }

    const char* getNumeroVol() const {
        return numeroVol;
    }

    OandD* getOd() const {
        return od;
    }

    std::string exportData() const {
        std::stringstream ss;
        ss << numeroVol << "," << od->exportData();
        return ss.str();
    }
    
    static void writeColumnNames(const std::string& fichier) {
            std::ofstream outFile(fichier, std::ofstream::out);
            if (outFile.is_open()) {
                outFile << "NumeroDeVol,Origine,Destination,MinimumFare,MaximumFare,MoyenneFare\n";
            } else {
                std::cerr << "Impossible d'ouvrir le fichier: " << fichier << std::endl;
            }
        }
};

std::vector<std::shared_ptr<OandD>> lireCSV(const std::string& fichier) {
    std::vector<std::shared_ptr<OandD>> ods;
    std::ifstream file(fichier);
    std::string line;

    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream s(line);
        std::string origine, destination, tarifStr;
        std::getline(s, origine, ';');
        std::getline(s, destination, ';');
        if (!std::getline(s, tarifStr) || tarifStr.empty()) {
            std::cerr << "Ligne invalide ou tarif manquant: " << line << std::endl;
            continue;
        }

        try {
            double tarifDouble = std::stod(tarifStr);
            long tarifLong = static_cast<long>(tarifDouble * 100);
            bool found = false;
            for (auto& od : ods) {
                if (strcmp(od->getOrigine(), origine.c_str()) == 0 && strcmp(od->getDestination(), destination.c_str()) == 0) {
                    od->addTarif(tarifLong);
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::set<long> tarifs{tarifLong};
                ods.push_back(std::make_shared<OandD>(origine.c_str(), destination.c_str(), tarifs));
            }
                        for (auto& od : ods) {
                            if (strcmp(od->getOrigine(), origine.c_str()) == 0 && strcmp(od->getDestination(), destination.c_str()) == 0) {
                                for (long tarif : od->getTarifs()) {
                                    std::cout << static_cast<double>(tarif) / 100.0 << " ";
                                }
                                break;
                            }
                        }
                        std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Erreur de conversion -> ligne: " << line << "; Erreur: " << e.what() << std::endl;
            continue;
        }
    }

    return ods;
}


void exporterODs(const std::vector<std::shared_ptr<OandD>>& ods, const std::string& fichier) {
    OandD::writeColumnNames(fichier);

    std::ofstream outFile(fichier, std::ofstream::app);
    for (const auto& od : ods) {
        if (outFile.is_open()) {
            outFile << od->exportData() << std::endl;
        } else {
            std::cerr << "Ecriture impossible dans: " << fichier << std::endl;
            break;
        }
    }
}

void exporterFlightNbr(const FlightNbr& flight, const std::string& fichier) {
        FlightNbr::writeColumnNames(fichier);
    
    std::ofstream outFile(fichier, std::ofstream::app);
    if (outFile.is_open()) {
        outFile << flight.exportData() << std::endl;
    } else {
        std::cerr << "Ecriture impossible dans: " << fichier << std::endl;
    }
}


int main() {
    std::string chemin1 = "/Users/julia/Desktop/entretien/airfrance/OrigineDestinationTarif.csv";
    std::string chemin2 = "/Users/julia/Desktop/entretien/airfrance/ExportODs.csv";

    auto ods = lireCSV(chemin1);

    exporterODs(ods, chemin2);

    std::cout << "Données exportées avec succès dans " << chemin2 << std::endl;

    return 0;
}

// test de la question 8
/*
int main() {
    std::string chemin1 = "/Users/julia/Desktop/entretien/airfrance/OrigineDestinationTarif.csv";
    std::vector<std::shared_ptr<OandD>> ods = lireCSV(chemin1);

    std::shared_ptr<OandD> odPtr = std::make_shared<OandD>("CDG", "EWR", std::set<long>{60076, 60021});
    FlightNbr flight("AF123", odPtr.get());

    std::string chemin2 = "/Users/julia/Desktop/entretien/airfrance/ExportFlightNbrs.csv";
    exporterFlightNbr(flight, chemin2);

    std::cout << "Données exportées avec succès dans " << chemin2 << std::endl;

    return 0;
}
*/
