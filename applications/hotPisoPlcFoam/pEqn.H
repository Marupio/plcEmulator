{
    volScalarField rUA = 1.0/UEqn.A();

    U = rUA*UEqn.H();
    phi = (fvc::interpolate(U) & mesh.Sf())
        + fvc::ddtPhiCorr(rUA, U, phi);

    adjustPhi(phi, U, p);

    // Non-orthogonal pressure corrector loop
    for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
    {
        // Pressure corrector

        fvScalarMatrix pEqn
        (
            fvm::laplacian(rUA, p) == fvc::div(phi)
        );

        pEqn.setReference(pRefCell, pRefValue);

        if
        (
            corr == nCorr-1
         && nonOrth == nNonOrthCorr
        )
        {
            pEqn.solve
            (
                mesh.solutionDict().solver("pFinal")
            );
        }
        else
        {
            pEqn.solve();
        }

        if (nonOrth == nNonOrthCorr)
        {
            phi -= pEqn.flux();
        }
    }

    #include "continuityErrs.H"

    U -= rUA*fvc::grad(p);
    U.correctBoundaryConditions();
}
