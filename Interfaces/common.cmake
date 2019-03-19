# ---- Choose the models to be compiled ---- 
SET(TestCompilationModelNames "Isotropic2;Riemann2" CACHE STRING "TestCompilationModelNames")

SET(StandardModelNames "Isotropic2;Isotropic3;Diagonal2;Diagonal3;Riemann2;Riemann3;ReedsShepp2;ReedsSheppForward2;Elastica2;Dubins2;ReedsShepp3;ReedsSheppForward3;IsotropicDiff2;DubinsExt2" CACHE STRING "ModelNames")

Set(ExperimentalModelNames "Riemann4;Riemann5;Elastica2_9;ElasticaExt2_5;ReedsSheppExt2;ReedsSheppForwardExt2;RiemannDiff2;RiemannLifted2_Periodic;AsymmetricQuadratic3;AsymmetricQuadratic3p1;HalfDisk2" CACHE STRING "ExperimentalModelNames")
Set(CustomModelNames "" CACHE STRING "CustomModelNames")

option(IncludeStandardModels "IncludeStandardModels" TRUE)
option(IncludeExperimentalModels "IncludeExperimentalModels" FALSE)

option(CustomExecutable "CustomExecutable")
option(TestCompilation "TestCompilation" FALSE)

Set(ModelNames ${CustomModelNames})

# Setting the model names
if(TestCompilation)
Set(ModelNames ${ModelNames} ${TestCompilationModelNames})
else()

	if(IncludeStandardModels)
	Set(ModelNames ${ModelNames} ${StandardModelNames})
	endif()

	if(IncludeExperimentalModels)
	Set(ModelNames ${ModelNames} ${ExperimentalModelNames})
	endif()

endif()


list(REMOVE_DUPLICATES ModelNames)
message(STATUS "Compiled models : ${ModelNames}")