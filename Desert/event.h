#pragma once

#include <list>
#include <memory>

namespace Desert
{
	template <typename TOwner, typename ...TArgs>
	class Event
	{
		friend TOwner;
		//TOwner* _owner;

		enum class Type
		{
			Class, Static
		};

		class Delegate
		{
			Type type;
		public:
			Delegate(Type type)
				:type{ type } {}
			Type GetType() const { return type; }
			virtual void Process(TArgs ...args) = 0;
		};

		template <typename TClass, typename TMethod>
		class ClassDelegate : public Delegate
		{
			TClass* object;
			TMethod method;

		public:
			ClassDelegate(TClass* object, TMethod method)
				: Delegate(Type::Class), object{ object }, method{ method } {}
			virtual void Process(TArgs ...args) override
			{
				(object->*method)(args...);
			}
			bool Compare(TClass* obj, TMethod m)
			{
				return obj == object && m == method;
			}
		};

		template <typename TProcedure>
		class StaticDelegate : Delegate
		{
			TProcedure procedure;

		public:
			StaticDelegate(TProcedure procedure)
				: Delegate(Type::Static), procedure{ procedure } {}
			virtual void Process(TArgs ...args) override
			{
				procedure(args...);
			}
			bool Compare(TProcedure proc)
			{
				return proc == procedure;
			}
		};

		std::list<std::shared_ptr<Delegate>> delegates;

		void operator () (TArgs... args)
		{
			for (auto& d : delegates)
			{
				d->Process(args...);
			}
		}

	public:
		/*Event(TOwner* owner)
			: _owner{ owner }
		{

		}*/

		template <typename TClass, typename TMethod>
		void operator += (std::pair<TClass*, TMethod> del)
		{
			std::shared_ptr<Delegate> d{ new ClassDelegate(del.first, del.second) };
			delegates.push_back(d);
		}

		template <typename TProcedure>
		void operator += (TProcedure del)
		{
			std::shared_ptr<Delegate> d{ new StaticDelegate(del) };
			delegates.push_back(d);
		}

		template <typename TClass, typename TMethod>
		void operator -= (std::pair<TClass*, TMethod> delegate)
		{
			for (auto& d : delegates)
			{
				if (d->GetType() == Type::Class)
				{
					auto p = std::static_pointer_cast<ClassDelegate<TClass, TMethod>>(d);
					if (p && p->Compare(delegate.first, delegate.second))
					{
						delegates.remove(d);
						return;
					}
				}
			}
		}

		template <typename TProcedure>
		void operator -= (TProcedure del)
		{
			for (auto& d : delegates)
			{
				if (d->GetType() == Type::Static)
				{
					auto p = std::static_pointer_cast<StaticDelegate<TProcedure>>(d);
					if (p && p->Compare(del))
					{
						delegates.remove(d);
						return;
					}
				}
			}
		}



	};
}
