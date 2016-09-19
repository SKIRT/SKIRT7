/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PARALLELDATACUBE_HPP
#define PARALLELDATACUBE_HPP

#include <memory>

class Array;
class PeerToPeerCommunicator;
class ProcessAssigner;
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** An object of this class represents a data cube of which the slices per wavelength are
    distributed across the different processes. It acts as a table indexed on wavelengths and
    pixels, with respectively the first and second index of its ()-operator. It makes use of the \c
    ProcessAssigner contained in the \c WavelengthGrid to check and convert the given wavelength
    index. At the end of a simulation, the data stored across the different processes can be
    gathered at the root process using an MPI communication. When the wavelengths are evenly
    divided across the processes, the memory usage per process is expected to scale as 1/N, with N
    the number of processes. When data parallelization is not active, there will be no wavelength
    assigner, and this object will store data for all wavelengths. */
class ParallelDataCube
{
    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. Before a ParallelDataCube object can be used, it first needs to be
        initialized (see \c initialize()). */
    ParallelDataCube();

    /** This function readies the ParallelDataCube for use. The number of pixels is given as a
        parameter. Using the given \c SimulationItem, this function gains access to the simulation
        hierarchy, allowing it to look for the \c WavelengthGrid and the \c PeerToPeerCommunicator. Via the
        wavelength grid, the wavelength assigner can be obtained. From the wavelength assigner, it
        is determined what size the \c _partialCube data member should be, and the necessary memory
        is allocated. */
    void initialize(size_t Nframep, SimulationItem* item);

    //======================== Other Methods =======================

    /** This function collects the data from the different processes into one big data cube at the
        root process. This result is returned through a shared pointer, so that ownership of the
        created data cube can be transferred and copying of the data is avoided. All processes need
        to call this function together, as it uses a collective MPI communication. In
        non-distributed mode, a summation is performed, adding the flux contributions from every
        process, and the result is stored at the root process. The processes other than the root do
        not need the complete cube, as only the root will write the data to disk. Therefore, the
        return value is a pointer to an empty array for all non-root processes. */
    std::shared_ptr<Array> constructCompleteCube();

    /** This operator provides writable access to the contents of the ParallelDataCube. First it is
        checked if the specified wavelength is available at the calling process. If this is not the
        case, a \c FATALERROR is thrown. Then, the index is converted using the wavelength
        assigner, and the correct element is retrieved. */
    double& operator()(int ell, int pixel);

    /** This operator provides read-only access to the contents of the ParallelDataCube. First it
        is checked if the specified wavelength is available at the calling process. If this is not
        the case, a \c FATALERROR is thrown. Then, the index is converted using the wavelength
        assigner, and the correct element is retrieved. */
    const double& operator()(int ell, int pixel) const;

    //======================== Data Members ========================

private:
    const ProcessAssigner* _wavelengthAssigner;
    PeerToPeerCommunicator* _comm;
    size_t _Nlambda;
    size_t _Nframep;

    std::shared_ptr<Array> _partialCube;
};

////////////////////////////////////////////////////////////////////

#endif // PARALLELDATACUBE_HPP
