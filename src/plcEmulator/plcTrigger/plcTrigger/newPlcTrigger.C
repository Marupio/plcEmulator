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

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::plcTrigger> Foam::plcTrigger::New
(
    plcEmulator& control,
    const word& triggerName,
    const dictionary& inputDict
)
{
    word triggerType(inputDict.lookup("type"));

    if (debug)
    {
        Info<< "plcTrigger::New(): type = " << triggerType
            << ", name = " << triggerName << endl;
    }

    dictionaryConstructorTable::iterator cstrIter =
        dictionaryConstructorTablePtr_->find(triggerType);

    if (cstrIter == dictionaryConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "plcTrigger::New"
        )   << "Trigger " << triggerName << " has an unknown plcTrigger "
            << "type " << triggerType << endl << endl
            << "Valid plcTrigger types are :" << endl
            << dictionaryConstructorTablePtr_->toc()
            << exit(FatalError);
    }

    return cstrIter()(control, triggerName, inputDict);
}

// ************************************************************************* //
