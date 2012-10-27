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

#include "solverSignalPlcTrigger.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(solverSignalPlcTrigger, 0);

    addToRunTimeSelectionTable
    (
        plcTrigger,
        solverSignalPlcTrigger,
        dictionary
    );
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::solverSignalPlcTrigger::solverSignalPlcTrigger
(
    plcEmulator& control,
    const word& triggerName,
    const dictionary& inputDict
)
:
    plcTrigger(control, triggerName, inputDict)
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::solverSignalPlcTrigger::evaluate(const Time& runTime)
{
    return value_;
}


void Foam::solverSignalPlcTrigger::toggle()
{
    value_ = !value_;
}


void Foam::solverSignalPlcTrigger::set(bool newValue)
{
    value_ = newValue;
}


void Foam::solverSignalPlcTrigger::readOutputDict
(
    const dictionary& outputSubDict
)
{
    value_ = readBool(outputSubDict.lookup("value"));
}


Foam::dictionary Foam::solverSignalPlcTrigger::writeOutputDict() const
{
    dictionary returnMe;
    returnMe.set("value", value_);
    return returnMe;
}

// ************************************************************************* //
