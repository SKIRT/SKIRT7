/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PROCESSMANAGER_HPP
#define PROCESSMANAGER_HPP

#include <atomic>

class ProcessManager
{
public:
    /* This function... */
    ProcessManager() = delete;

    /** This static function initializes the MPI library used for remote communication, if present.
        The function must be called exactly once at the very start of the program. It is passed a
        reference to the command line arguments to provide the MPI library with the opportunity to
        use and/or adjust them (e.g. to remove any MPI related arguments). */
    static void initialize(int *argc, char ***argv);

    /** This static function finalizes the MPI library used for remote communication, if present.
        The function must be called exactly once at the very end of the program. */
    static void finalize();

    /** This function .. */
    static void acquireMPI(int& Nprocs, int&rank);

    /** This function .. */
    static void releaseMPI();

    /** This function .. */
    static void barrier();

    /** This function .. */
    static void sum(double* my_array, double* result_array, int nvalues, int root);

    /** This function .. */
    static void sum_all(double* my_array, double* result_array, int nvalues);

    /** This function .. */
    static bool isRoot();

    /** This function .. */
    static bool isMultiProc();

private:

    /* This... */
    static std::atomic<int> requests;

};

#endif // PROCESSMANAGER_HPP
