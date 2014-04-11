/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef FOAMPARTITION_HPP
#define FOAMPARTITION_HPP

////////////////////////////////////////////////////////////////////

/** The FoamPartition class is used by the Foam class for looping over partitions. This class is part
    of the foam classes that have been copied from an external library and adjusted to fit the
    SKIRT mold. There is no documentation other than the sparse comments in the source code. */
class FoamPartition
{
private:
    int m_len;                  // length=dimension
    int* m_digits;              // [m_len] Partition
public:
    FoamPartition(int len);
    ~FoamPartition();
    void Reset();
    int Next();
    int Digit(int i);
private:
    FoamPartition();
    FoamPartition(const FoamPartition&);
    FoamPartition& operator=(const FoamPartition&);
};

/////////////////////////////////////////////////////////////////////////////

#endif // FOAMPARTITION_HPP
