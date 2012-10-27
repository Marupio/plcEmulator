/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline const multiSolver& plcEmulator::multiRun() const
{
    return multiRun_;
}


inline multiSolver& plcEmulator::multiRun()
{
    return multiRun_;
}


inline const IOdictionary& plcEmulator::plcEmulatorDict() const
{
    return plcEmulatorDict_;
}


inline const dictionary& plcEmulator::outputDict() const
{
    return outputDict_;
}


inline const word& plcEmulator::outputDictName() const
{
    return outputDictName_;
}


inline const PtrList<plcTrigger>& plcEmulator::triggers() const
{
    return triggers_;
}

inline PtrList<plcTrigger>& plcEmulator::triggers()
{
    return triggers_;
}


inline const PtrList<plcLogic>& plcEmulator::logic() const
{
    return logic_;
}


inline PtrList<plcLogic>& plcEmulator::logic()
{
    return logic_;
}


inline const wordList& plcEmulator::solverDomains() const
{
    return solverDomains_;
}


inline const wordList& plcEmulator::algorithms() const
{
    return algorithms_;
}


inline const labelListList& plcEmulator::algorithmGroups() const
{
    return algorithmGroups_;
}


inline const label& plcEmulator::currentSolverDomainIndex() const
{
    return currentSolverDomain_;
}


inline word plcEmulator::currentSolverDomainName() const
{
    return solverDomains_[currentSolverDomain_];
}


inline const label& plcEmulator::currentAlgorithmIndex() const
{
    return currentAlgorithm_;
}


inline word plcEmulator::currentAlgorithmName() const
{
    return algorithms_[currentAlgorithm_];
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
