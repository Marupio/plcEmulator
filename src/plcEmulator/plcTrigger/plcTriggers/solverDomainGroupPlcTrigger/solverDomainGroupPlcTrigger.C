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

#include "solverDomainGroupPlcTrigger.H"
#include "addToRunTimeSelectionTable.H"
#include "plcEmulator.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(solverDomainGroupPlcTrigger, 0);

    addToRunTimeSelectionTable
    (
        plcTrigger,
        solverDomainGroupPlcTrigger,
        dictionary
    );
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::solverDomainGroupPlcTrigger::solverDomainGroupPlcTrigger
(
    plcEmulator& control,
    const word& triggerName,
    const dictionary& inputDict
)
:
    plcTrigger(control, triggerName, inputDict),
    solverDomainNames_(inputDict.lookup("solverDomains")),
    solverDomainIndices_(solverDomainNames_.size(), -1)
{
    const wordList& allSolverDomains(control.solverDomains());
    forAll(solverDomainNames_, sgi)
    {
        const word& subGroupName(solverDomainNames_[sgi]);
        forAll(allSolverDomains, asi)
        {
            const word& testName(allSolverDomains[asi]);
            if (subGroupName == testName)
            {
                solverDomainIndices_[sgi] = asi;
                break;
            }
        }
        if (solverDomainIndices_[sgi] < 0)
        {
            FatalIOErrorIn
            (
                "solverDomainGroupPlcTrigger::solverDomainGroupPlcTrigger",
                inputDict
            ) << "Solver domain '" << subGroupName << "' is not recognized. "
                << "Available solver domains are: " << allSolverDomains
                << exit(FatalIOError);
        }
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::solverDomainGroupPlcTrigger::evaluate(const Time& runTime)
{
    const label& currentSolverDomainIndex(control_.currentSolverDomainIndex());
    forAll(solverDomainIndices_, sdi)
    {
        if (solverDomainIndices_[sdi] == currentSolverDomainIndex)
        {
            value_ = true;
            return value_;
        }
    }
    value_ = false;
    return value_;
}

// ************************************************************************* //
