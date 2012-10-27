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
    Foam::conditionalSwitchPlcTrigger

Description
    A plcTrigger that is true or false depending on the condition of other
    user-specified triggers
    
SourceFiles
    conditionalSwitchPlcTrigger.C

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/

#ifndef conditionalSwitchPlcTrigger_H
#define conditionalSwitchPlcTrigger_H

#include "plcTrigger.H"
#include "UPtrList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class conditionalSwitchPlcTrigger Declaration
\*---------------------------------------------------------------------------*/

class conditionalSwitchPlcTrigger
:
    public plcTrigger
{
    // Private member data
    
        //- Reference to the inputDict
        const dictionary& inputDict_;

        //- True when inputDict has been read (it is delayed because all
        //  triggers have to be known before hand)
        bool inputDictWasRead_;
            
        //- Switching on - required true triggers
        UPtrList<const plcTrigger> onTriggered_;
        
        //- Switching on - required false triggers
        UPtrList<const plcTrigger> onNotTriggered_;

        //- Switching off - required true triggers
        UPtrList<const plcTrigger> offTriggered_;
        
        //- Switching off - required false triggers
        UPtrList<const plcTrigger> offNotTriggered_;

    // Private member functions

        //- Read input dictionary
        void readDict();

public:

    //- Runtime type information
    TypeName("conditionalSwitch");


    // Constructors

        //- Construct from dictionary, given name
        conditionalSwitchPlcTrigger
        (
            plcEmulator& control,
            const word& triggerName,
            const dictionary& inputDict
        );

    // Destructor

        virtual ~conditionalSwitchPlcTrigger()
        {}


    // Member functions

        // Evaluation

            //- Evaluate
            virtual bool evaluate(const Time& runTime);

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
