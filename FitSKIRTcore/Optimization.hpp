/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef OPTIMIZATION_HPP
#define OPTIMIZATION_HPP

#include "AdjustableSkirtSimulation.hpp"
#include "GAPopulation.h"
#include "GARealGenome.h"
#include "GASStateGA.h"
#include "SimulationItem.hpp"
#include <QVector>
#include <fstream>

////////////////////////////////////////////////////////////////////

/** The optimization class contains all information necessary to do the acutal fitting.
    This class uses the genetic algorithm library, GAlib. The ParameterRanges object from the OligoFitScheme is
    used to set the boundaries and to interpret the output values. The popevaluate function present in this document
    is used by the optimization library and feeds the genome values to the OligoFitScheme object in the form of a
    ReplacementDict. This is done in parallalel for all individuals over the amount of available threads. */
class Optimization: public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "The optimization setup")

    Q_CLASSINFO("Property", "popsize")
    Q_CLASSINFO("Title", "the number of individuals in the population")
    Q_CLASSINFO("Default", "100")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1000")

    Q_CLASSINFO("Property", "generations")
    Q_CLASSINFO("Title", "the number of generations to be evaluated ")
    Q_CLASSINFO("Default", "100")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1000")

    Q_CLASSINFO("Property", "pmut")
    Q_CLASSINFO("Title", "the mutation probability")
    Q_CLASSINFO("Default", "0.03")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")

    Q_CLASSINFO("Property", "pcross")
    Q_CLASSINFO("Title", "the crossover rate")
    Q_CLASSINFO("Default", "0.65")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Optimization();

protected:
    /** This function sets up SimulationItem */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function sets the populationsize. */
    Q_INVOKABLE void setPopsize(int value);

    /** This function returns the populationsize. */
    Q_INVOKABLE int popsize() const;

    /** This function sets the populationsize. */
    Q_INVOKABLE void setGenerations(int value);

    /** This function returns the populationsize. */
    Q_INVOKABLE int generations() const;

    /** This function sets the populationsize. */
    Q_INVOKABLE void setPmut(double value);

    /** This function returns the populationsize. */
    Q_INVOKABLE double pmut() const;

    /** This function sets the populationsize. */
    Q_INVOKABLE void setPcross(double value);

    /** This function returns the populationsize. */
    Q_INVOKABLE double pcross() const;

    //======================== Other Functions =======================

    /** Checks if the optimization process is done. */
    bool done();

    /** Initializes the GA library. */
    void initialize();

    /** Sets the \f$\chi^2\f$ values, and luminosities and returns it in a QVariant object. */
    QVariant chi2(QVariant input);

    /** Evaluates all individuals of a certain population. This is done by creating a temporary folder to store all
        simulations. The individual evaluations are parallelised over the available number of threads and the function
        values are stored. At the end of each generation the temporary folder is removed, the scores for each
        individual are set and the best solutions are stored. */
    void PopEvaluate(GAPopulation & p);

    /** Proceed one step in the optimization process. */
    void step();

    /** Translates variables to QVariant and performs the chi2 funtion in parallel. */
    void splitChi();

    /** Write out a list of doubles to the output file. */
    void writeList(std::ofstream *stream, QList<double> list);

    /** Write out the current genome to the best simulations file. */
    void writeBest(int index, int consec);

    /** Write out an entire line. */
    void writeLine(std::ofstream *stream, int i);

    /** Clears the generation information. Removes the temporary folder. */
    void clearGen(const QString & dirName);

    //======================== Data Members ========================

private:
    int _popsize;
    int _generations;
    int _consec;
    double _pmut;
    double _pcross;
    double _bestChi2;
    GARealAlleleSetArray _allelesetarray;
    GARealGenome* _genome;
    GASteadyStateGA* _ga;
    std::ofstream _stream;
    std::ofstream _beststream;
    QList<AdjustableSkirtSimulation::ReplacementDict *> _genReplacement;
    QList<int> _genIndices;
    QVector<double> _genScores;
    QList<QVector<double> > _genValues;
    QList<QVector<double> > _genUnitsValues;
    QVector<QList<double> > _genLum;
    QVector<QList<double> > _genChis;

};

////////////////////////////////////////////////////////////////////

#endif // OPTIMIZATION_HPP
