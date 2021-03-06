// Copyright 2016 Timothy Davison, all rights reserved.

#pragma once

#include <array>
#include <mutex>
#include <unordered_map>
#include <assert.h>

template<class T> struct Constructor {

	template <class U> struct Signature { typedef T type; };

	T operator()() const {
		return T();
	}

	template<class A>
	T operator()( A& a ) const {
		return T( a );
	}
};

template<typename TBase>
struct StructTypeManager
{
	typedef int32 TypeId;	// this needs to become a class so that we can prevent users from creating their own TypeIds

	template<typename TComponent>
	static TypeId typeId()
	{
		static_assert(std::is_base_of<TBase, TComponent>::value, "TClass must be derived from TBase");

		const static TypeId s_id = typeForStruct( TComponent::StaticStruct() );
		
		return s_id;
	}

	// Returns the UScriptStruct associated with the provided TypeId
	// @note This is possible, because TypeId are only generated by
	// typeId or typeForStruct.
	static UScriptStruct* structForType( TypeId type )
	{
		assert( type < _typeToStruct.size() );// we encountered an unregistered type-struct pair, call typeForStruct first" 

		UScriptStruct * typeStruct = _typeToStruct[type];

		return typeStruct;  
	}

	static TypeId typeForStruct( UScriptStruct * typeStruct )
	{
		auto found = _structToType.find( typeStruct );

		if(found == _structToType.end())
		{
			TypeId id = _nextId++;

			_structToType.emplace( typeStruct, id );

			if(id >= _typeToStruct.size())
				_typeToStruct.resize( id + 1 );  
			_typeToStruct[id] = typeStruct;

			return id;
		}
		else
			return found->second;
	}

private:
	static TypeId _nextId;
	static std::unordered_map<UScriptStruct*, TypeId> _structToType;
	static std::vector<UScriptStruct*> _typeToStruct;
};

template<typename TBase>
typename StructTypeManager<TBase>::TypeId StructTypeManager<TBase>::_nextId = 0;

template<typename TBase>
typename std::unordered_map<UScriptStruct*, typename StructTypeManager<TBase>::TypeId> StructTypeManager<TBase>::_structToType;

template<typename TBase>
typename  std::vector<UScriptStruct*> StructTypeManager<TBase>::_typeToStruct;


// ------------------------------------------------------------------------------
// In this variant of the ClassTypeId, TClass does not have to inherit from TBase
// ------------------------------------------------------------------------------

template<typename TBase>
struct UncheckedTypeId
{
public:
	typedef std::size_t TypeId;

	template<typename TClass>
	static TypeId typeId()
	{
		static const TypeId id = _nextId++;

		return id;
	}

private:
	static TypeId _nextId;
};

template <typename TBase>
typename UncheckedTypeId<TBase>::TypeId UncheckedTypeId<TBase>::_nextId = 0;