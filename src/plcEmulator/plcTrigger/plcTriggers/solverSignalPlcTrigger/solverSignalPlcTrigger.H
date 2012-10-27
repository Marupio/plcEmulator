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

Class
    Foam::solverSignalPlcTrigger

Description
    A plcTrigger that is true when the current solver domain is a member of a
    user-defined group of solver domains
    
SourceFiles
    solverSignalPlcTrigger.C

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/

#ifndef solverSignalPlcTrigger_H
#define solverSignalPlcTrigger_H

#include "plcTrigger.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class solverSignalPlcTrigger Declaration
\*---------------------------------------------------------------------------*/

class solverSignalPlcTrigger
:
    public plcTrigger
{
public:

    //- Runtime type information
    TypeName("solverSignal");


    // Constructors

        //- Construct from dictionary, given name
        solverSignalPlcTrigger
        (
            plcEmulator& control,
            const word& triggerName,
            const dictionary& inputDict
        );

    // Destructor

        virtual ~solverSignalPlcTrigger()
        {}


    // Member functions

        // Evaluation

            //- Evaluate
            virtual bool evaluate(const Time& runTime);

            //- Change the value
            void toggle();

            //- Set the value
            void set(bool newValue);

        // Read and write
        
            //- Read the outputDict, used to get latest values for local data
            //  when resuming a simulation mid-run
            virtual void readOutputDict(const dictionary& outputSubDict);
            
            //- Write local data to a dictionary
            virtual dictionary writeOutputDict() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
