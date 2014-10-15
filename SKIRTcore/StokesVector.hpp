/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef STOKESVECTOR_HPP
#define STOKESVECTOR_HPP

//////////////////////////////////////////////////////////////////////

/** An object of the StokesVector class describes the polarization state of a photon (package), and
    offers functions to apply certain transformations to it. Specifically, a Stokes vector contains
    the four Stokes parameters $I$, $Q$, $U$, and $V$. The first parameter ($I$) indicates the
    total intensity, and the remaining parameters ($Q$, $U$, and $V$) represent various degrees of
    linear and circular polarization. Stokes parameters have a dimension of intensity. However, in
    our implementation, the parameters are normalized to dimensionless values through division by
    $I$. Consequently $I=1$ at all times, so that this parameter does not need to be stored. */
class StokesVector
{
public:

    // -------- constructors and setters ----------

    /** The default constructor initializes the Stokes vector to an unpolarized state. */
    StokesVector() : _Q(0), _U(0), _V(0) { }

    /** This function sets the Stokes vector to an unpolarized state. */
    void clearStokes() { _Q = 0; _U = 0; _V = 0; }

    /** This function sets the Stokes vector to a copy of the specified Stokes vector. */
    void setStokes(const StokesVector* sv) { _Q = sv->_Q; _U = sv->_U; _V = sv->_V; }

    /** This function sets the Stokes vector to the specified parameter values, after normalizing
        them through division by $I$. If $I=0$, the Stokes vector is set to an unpolarized state.
        */
    void setStokes(double I, double Q, double U, double V);

    // ------------ getters --------------

    /** This function returns the Stokes parameter I, which is always equal to one. */
    double stokesI() const { return 1.; }

    /** This function returns the Stokes parameter Q. */
    double stokesQ() const { return _Q; }

    /** This function returns the Stokes parameter U. */
    double stokesU() const { return _U; }

    /** This function returns the Stokes parameter V. */
    double stokesV() const { return _V; }

    /** This function returns the Stokes parameters in the specified arguments. */
    void stokes(double& I, double& Q, double& U, double& V) { I = 1.; Q = _Q; U = _U; V = _V; }

    // -------- calculated properties -------

    /** This function returns the total polarization degree for the Stokes vector. */
   double totalPolarizationDegree() const;

    /** This function returns the linear polarization degree for the Stokes vector. */
   double linearPolarizationDegree() const;

    /** This function returns the polarization position angle in radians for the Stokes vector. */
    double polarizationAngle() const;

    // ------------ transformations -----------

    /** This function rotates the Stokes vector by the specified angle around the current flight
        direction. */
    void rotateStokes(double alpha);

    /** This function transforms the polarization state described by this Stokes vector by applying
        the Mueller matrix with the specified coefficients (and zero elements elsewhere) to its
        existing state. */
    void applyMueller(double S11, double S12, double S33, double S34);

private:
    double _Q, _U, _V;
};

//////////////////////////////////////////////////////////////////////

#endif // STOKESVECTOR_HPP
