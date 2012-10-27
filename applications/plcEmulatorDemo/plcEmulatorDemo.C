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

Application
    plcTester

Description
    A bunch of bogus algorithms designed for the testing of the plcController.

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "multiSolver.H"
#include "plcController.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{

#   include "setRootCase.H"

    multiSolver multiRun
    (
        multiSolver::multiControlDictName,
        args.rootPath(),
        args.caseName()
    );

    word solverDomain;
    word algorithm;

    // Find initial solver domain
    timeCluster tcSource(multiRun.initialDataSource());

    solverDomain = tcSource.solverDomainName();
    if (solverDomain == "default")
    {
        solverDomain = multiRun.startDomain();
        if (solverDomain == "default")
        {
            FatalIOErrorIn("plcTester::main", multiRun.multiControlDict())
                << "Cannot determine start solverDomain. Use keyword "
                << "'startDomain' or change 'initialStartFrom'."
                << exit(FatalIOError);
        }
    }

    // Initialize solver domain
    multiRun.setSolverDomain(solverDomain);
    bool bugWorkAround(true);

    // Create controller, reads current solverDomain and triggers
    plcController control(multiRun);

    // Main loop (superLoop)
    while (multiRun.run())
    {
        algorithm = control.currentAlgorithmName();
        solverDomain = control.currentSolverDomainName();
        if (!bugWorkAround)
        {
            multiRun.setSolverDomain(solverDomain);
        }
        bugWorkAround = false;

        if (multiRun.run())
        {
            if (algorithm == "algorithm0")
            {
                Info << "*** Switching to solverDomain " << solverDomain
                    << " with algorithm " << algorithm << " ***\n" << endl;
#               include "algorithm0.H"
            }
            else if (algorithm == "algorithm1")
            {
                Info << "*** Switching to solverDomain " << solverDomain
                    << " with algorithm " << algorithm << " ***\n" << endl;
#               include "algorithm1.H"
            }
            else if (algorithm == "algorithm2")
            {
                Info << "*** Switching to solverDomain " << solverDomain
                    << " with algorithm " << algorithm << " ***\n" << endl;
#               include "algorithm2.H"
            }
            else if (algorithm == "algorithm3")
            {
                Info << "*** Switching to solverDomain " << solverDomain
                    << " with algorithm " << algorithm << " ***\n" << endl;
#               include "algorithm3.H"
            }
            else
            {
                FatalErrorIn("plcTester::main")
                    << "Unrecognized algorithm, '" << algorithm << "'.  Expecting "
                    << "one of: ( algorithm0 algorithm1 algorithm2 )"
                    << abort(FatalError);
            }
            multiRun++;
        }
    }

    Info<< "End\n" << endl;

    return(0);
}

// ************************************************************************* //
