//
//  DynamicFactoring.h
//  FileHFM
//
//  Created by Jean-Marie Mirebeau on 22/08/2018.
//

#ifndef Factoring_h
#define Factoring_h

#include "Base/HamiltonFastMarching.h"

/**
 This class implements dynamic factoring, a technique used to mitigate the impact of the singularities of the solution on the accuracy of the numerical scheme.
 Such singularities arise
 - at the seed points.
 - behind obstacle corners.
 
 While the improvement obtained from including the seed points is obvious, the usefulness of treating the obstacles corners is less clear. Indeed, in the later case, a real improvement is only seen if the obstacles boundaries are aligned with the grid.

 */

/*
 In some rare edge cases, factorization could be slightly less effective than anticipated due to implementation:
- Seed on the boundary of a domain equipped with periodic boundary conditions. (Additional h^2 error)
- Eulerian models with wide initialization around seed, used with dynamic factoring. (Additional h or h^2 error)
 
*/

// TODO : Complete static case. (Setup, SetupIndexStatic)
// TODO ? boolean set to false if no guess made, to speed up in useless case
// TODO : first order differentiation w.r.t. position in third order correction
// TODO : Static case with periodic boundary conditions ? (With repeated sources ?)

enum class FactoringPointChoice {Key, Current, Both};

template<typename T>
struct Factoring {
    typedef T Traits;
    typedef HamiltonFastMarching<Traits> HFM;
    Redeclare15Types(HFM,IndexCRef,FullIndexCRef,DiscreteFlowType,OffsetType,
					 ScalarType,DiscreteType,IndexType,IndexDiff,DomainTransformType,
					 VectorType,HFMI,PointType,DistanceGuess,DiscreteFlowElement,DomainType)
    Redeclare1Constant(HFM,Dimension)
    template<typename E, size_t n> using Array = typename Traits::template Array<E,n>;
	
	bool NeedsRecompute(IndexCRef);
	bool SetIndexStatic(IndexCRef);
    bool SetIndexDynamic(IndexCRef,const DiscreteFlowType &);
    ScalarType Correction(const OffsetType &, int) const;
	
	std::vector<std::pair<PointType,DistanceGuess> > factoringCenters; // Adimensionized quantities
    ScalarType factoringRadius = 10; // Adimensionized quantity
    Array<bool, Dimension> factoringRegion;
    FactoringPointChoice pointChoice = FactoringPointChoice::Key;
	FactoringMethod	method = FactoringMethod::None;
    bool Setup(HFMI *);
protected:
    const HFM * pFM=nullptr;
	
	// Only for pointChoice==Current or pointChoice==Both, setup by SetIndexStatic and SetIndexDynamic
	IndexType currentIndex;
	DistanceGuess currentGuess;
	
	struct ElementaryGuess;
	
	// Dynamic factoring only
	struct {
		// Generated by Setup
		std::map<DiscreteType,int> factoringKeypoints;
		Array<bool, Dimension> factoringDone;
		std::multimap<DiscreteType,std::pair<OffsetType,ScalarType> > factoringNeighbors;
		
		// Generated by SetupIndexDynamic
		std::vector<std::pair<ScalarType,ElementaryGuess> > guesses;
		std::vector<std::pair<OffsetType,ScalarType> > currentNeighbors, currentNeighbors2; // cache
	} data_dynamic;
	
	// Static factoring only
	ElementaryGuess data_static;
	
	ScalarType CorrectionCurrent(const OffsetType &, int, const VectorType &) const;
	ScalarType CorrectionKey(const OffsetType &, int, const ElementaryGuess &) const;
	
    void MakeFactor(FullIndexCRef,const DiscreteFlowType &);
    bool MakeGuess(FullIndexCRef);
    
    // Used at setup only
    void SetupCenters(HFMI*);
	void SetupDijkstra();
	void SetupRegion(HFMI*);
    std::vector<std::pair<IndexDiff,ScalarType> > edges;
    bool OnBoundary(IndexCRef,const DomainType &) const;
	
//	struct PoincareExact; mutable bool useExact=false;// DEBUG
};

template<typename T>
struct Factoring<T>::ElementaryGuess {
	int centerIndex = -1; // Index in factoringCenters table
	VectorType base; // vector from current point to center
	DomainTransformType transform; // transformation from current point to center
	PrintSelfMacro(ElementaryGuess)
};



#include "Implementation/DynamicFactoring.hxx"

#endif /* DynamicFactoring_h */
