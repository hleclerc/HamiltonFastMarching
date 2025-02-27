//
//  Seismic2.h
//  FileHFM
//
//  Created by Jean-Marie Mirebeau on 30/01/2019.
//

#ifndef Seismic2_h
#define Seismic2_h

#include <unordered_map>

#include "Base/Lagrangian2Stencil.h"
#include "Specializations/CommonTraits.h"

#include "JMM_CPPLibs/LinearAlgebra/SeismicNorm.h"
#include "JMM_CPPLibs/LinearAlgebra/ComposedNorm.h"
#include "JMM_CPPLibs/LinearAlgebra/VectorPairType.h"


struct TraitsSeismic2 : TraitsBase<2> {
	using StencilType = Lagrangian2Stencil<OffsetType,ScalarType,DiscreteType>;
	using DomainType = PeriodicGrid<TraitsSeismic2>;
	struct DifferenceType {static const int multSize = -1; struct MultiplierType {};};
	
	using NormType = LinearAlgebra::SeismicNorm<ScalarType,2>;
	using DistanceGuess = NormType;
	
	using MetricElementType = NormType::HookeTensorType;
	static NormType MakeNorm(const MetricElementType & m, ScalarType h){
		return NormType{(1./square(h))*m};}
};

struct TraitsSeismicTopographic2 : TraitsBase<2> {
	
	using StencilType = Lagrangian2Stencil<OffsetType,ScalarType,DiscreteType>;
	using DomainType = PeriodicGrid<TraitsSeismicTopographic2>;
	struct DifferenceType {static const int multSize = -1; struct MultiplierType {};};

	using BaseNormType = LinearAlgebra::SeismicNorm<ScalarType,2>;
	using TransformType = LinearAlgebra::TopographicTransform<VectorType>;
	using NormType = LinearAlgebra::ComposedNorm<BaseNormType,TransformType>;
	using DistanceGuess = NormType;
	
	using MetricElementType = LinearAlgebra::VectorPair<BaseNormType::HookeTensorType,VectorType>;
	static NormType MakeNorm(const MetricElementType & m, ScalarType h){
		return NormType{(1./square(h))*m.first, TransformType{m.second}};}
	
};

template<typename TTraits>
struct StencilGenericLag2 final
: HamiltonFastMarching<TTraits>::StencilDataType {
	using Traits = TTraits;
	using HFM = HamiltonFastMarching<Traits>;
	using Superclass = typename HFM::StencilDataType;
	Redeclare13Types(HFM,ParamDefault,ParamInterface,HFMI,DiscreteFlowType,
					IndexCRef,VectorType,ScalarType,DiscreteType,OffsetCRef,RecomputeType,
					DomainType,IndexDiff,PointType)
	Redeclare6Types(Traits,NormType,IndexType,StencilType,OffsetType,DistanceGuess,
					MetricElementType)
	Redeclare1Type(Superclass,OffsetVal3)
	Redeclare1Constant(HFM,Dimension)
	
	// Specific to this model
	virtual std::pair<ScalarType,int> HopfLaxUpdate(IndexCRef, const OffsetVal3 &) override;
	virtual RecomputeType HopfLaxRecompute(IndexCRef,DiscreteFlowType &) override;
//	using MetricElementType = NormType::HookeTensorType;
	
	// Generic
	using MetricType = typename Traits::template DataSource<MetricElementType>;
	std::unique_ptr<MetricType> pMetric;
	ParamDefault param;
	
	ScalarType cosAngleMin = 0.5; // Refinement criterion
	virtual void SetNeighbors(IndexCRef index, std::vector<OffsetType> & stencil) override;
	virtual const ParamInterface & Param() const override {return param;}
	virtual void Setup(HFMI *) override;
	virtual DistanceGuess GetGuess(const PointType & p) const override;
	virtual DistanceGuess GetGuess(const IndexType & index) const override {return GetNorm(index);}
private:
	NormType GetNorm(IndexCRef index) const; // Includes rescaling by h
	
	std::vector<OffsetType> tmp_stencil;
	std::vector<VectorType> tmp_stencil_vec;
	std::vector<ScalarType> tmp_stencil_scal;
	template<size_t n> using Vec = typename NormType::template Vec<n>;
	
	// Tentative optimization : avoid recomputing gradients.
	// Actually counter productive : 50% more cpu time, cost of memory allocations exceeds gains.
	const bool useCache = false;
	std::unordered_map<long,VectorType> vertexCache;
	std::unordered_multimap<DiscreteType,long> vertexCacheKeys;
	virtual void EraseCache(DiscreteType index) override final;
	static long hash(DiscreteType,OffsetType);
};

using StencilSeismic2 = StencilGenericLag2<TraitsSeismic2>;
using StencilSeismicTopographic2 = StencilGenericLag2<TraitsSeismicTopographic2>;

#include "Implementation/Seismic2.hpp"


#endif /* Seismic2_h */
