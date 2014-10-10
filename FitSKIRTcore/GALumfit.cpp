/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "GALumfit.hpp"
#include "FatalError.hpp"
#include <math.h>
#include <iostream>

using namespace std;

////////////////////////////////////////////////////////////////////

float Objective(GAGenome & g, QList<Array> *sim)
{

    //reading the array list and taking out the reference image in the back
    Array ref = sim->last();
    sim->pop_back();
    GARealGenome& genome = (GARealGenome&)g;
    if (sim->size() != genome.size()) throw FATALERROR("Number of luminosities and components do not match");

    //reading the suggested luminosities for each component
    QList<double> lumis;
    for(int i=0; i<genome.length(); i++){
      lumis.append(pow(10,genome.gene(i)));
    }

    //determining the chi2 value for this genome
    int arraysize = (*sim)[0].size();
    double chi=0;

    for (int m=0; m<arraysize; m++)
    {
        double total_sim =  0;
        //create the correct summed image and take over masks from the reference image
        for(int n=0; n<sim->size(); n++)
        {
            if (ref[m]==0)
            {
                ((*sim)[n])[m] = 0;
            }
            else
            {
                total_sim += lumis[n] * (((*sim)[n])[m]);
            }
        }

        //calculate the chi2 value if for non masked regions
        double sigma = sqrt(abs(ref[m]) + total_sim);
        if (ref[m]==0)
        {
            total_sim = 0;
            sigma = 0;
        }
        else
        {
            chi += pow( (ref[m] - total_sim) / sigma,2);
        }

    }

    //returning the reference frame in the back of the list
    sim->append(ref);
    return chi;
}
//////////////////////////////////////////////////////////////////////

void Evaluator(GAPopulation & p)
{
    QList<Array> *sim = (QList<Array> *)p.userData();

    //loop over all individuals and make replacement for all unevaluated individuals
    for (int i=0; i<p.size(); i++)
    {
        if (p.individual(i).isEvaluated()==gaFalse)
        {
            double value =  Objective(p.individual(i),sim);
            p.individual(i).score(value);
        }
    }

}

////////////////////////////////////////////////////////////////////

GALumfit::GALumfit()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void GALumfit::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void GALumfit::setFixedSeed(bool value)
{
    _fixedSeed = value;
}

////////////////////////////////////////////////////////////////////

bool GALumfit::fixedSeed() const
{
    return _fixedSeed;
}

////////////////////////////////////////////////////////////////////

void GALumfit::setMinLuminosities(QList<double> value)
{
    _minLum = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> GALumfit::minLuminosities() const
{
    return _minLum;
}

//////////////////////////////////////////////////////////////////////

void GALumfit::setMaxLuminosities(QList<double> value)
{
    _maxLum = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> GALumfit::maxLuminosities() const
{
    return _maxLum;
}

////////////////////////////////////////////////////////////////////

void GALumfit::optimize(const Array *Rframe, QList<Array> *frames, QList<double> *lumis, double &chi2)
{

    //set the reference frame and frames list
    _ref = Rframe;
    QList<Array> *sim = frames;
    frames = sim;

    //create the boundaries, set to be uniform in logscale
    GARealAlleleSetArray allelesetarray;
    for(int i=0;i<_minLum.size();i++)
    {
        GARealAlleleSet RealAllele(log10(_minLum[i]),log10(_maxLum[i]));
        allelesetarray.add(RealAllele);
    }

    //set the initializers, mutator and crossover scheme
    GARealGenome* genome = new GARealGenome(allelesetarray);
    genome->initializer(GARealGenome::UniformInitializer);
    genome->mutator(GARealGaussianMutator);
    genome->crossover(GARealUniformCrossover);
    GASteadyStateGA* _ga= new GASteadyStateGA(*genome);
    GASigmaTruncationScaling scaling;
    _ga->minimize();
    GAPopulation popu = _ga->population();
    sim->append(*Rframe);
    popu.userData(sim);
    popu.evaluator(Evaluator);

    //set the populationsize and generations to scale with the number of components
    _ga->population(popu);
    _ga->populationSize(frames->size()*30);
    _ga->nGenerations(frames->size()*20);
    _ga->pMutation(0.03);
    _ga->pCrossover(0.65);
    _ga->scaling(scaling);
    _ga->scoreFrequency(0);  // was 1e-22 implicitly converted to integer 0
    _ga->selectScores(GAStatistics::AllScores);
    _ga->flushFrequency(0);  // was 1e-22 implicitly converted to integer 0
    if(_fixedSeed) _ga->initialize(4357);
    else _ga->initialize();

    //loop over the GA untill it is done and print out the best fitting values
    GARealGenome& best_genome = (GARealGenome&) _ga->statistics().bestIndividual();
    while(!_ga->done())_ga->step();
    for(int i=0;i<_minLum.size();i++) lumis->append(pow(10,best_genome.gene(i)));
    chi2=best_genome.score();


}

////////////////////////////////////////////////////////////////////
