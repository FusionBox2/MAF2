#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <memory>
#include <sstream>
#include <typeinfo>
#include "historic_map.h"
//#include "Core/Property.h"

#define ftkCommonTypeMacro(className) \
  public: \
  \
    typedef className Thisclass; \
  \
    typedef       Thisclass*        Pointer; \
    typedef const Thisclass*        ConstPointer; \
    typedef       Pointer           PointerArg; \
    typedef  ConstPointer           ConstPointerArg; \
  \
    static const std::type_info&    TypeId(); \
    static const std::string&       TypeName(); \
    virtual const std::type_info&   GetTypeId() const; \
    virtual const std::string&      GetTypeName() const; \
    virtual bool                    IsA(const std::type_info& type) const; \
    virtual bool                    IsA(const std::string& type) const; \
  \
    static Thisclass::Pointer       SafeDownCast(Rootclass::PointerArg o); \
    static Thisclass::ConstPointer  SafeDownCast(Rootclass::ConstPointerArg o); \
  \
    static Rootclass::Pointer       FactoryNew(); \
    static Thisclass::Pointer       New(); \
    static std::unique_ptr<Rootclass>      FactoryNew_() {return std::unique_ptr<Rootclass>(FactoryNew());} \
    static std::unique_ptr<Thisclass>      New_() {return std::unique_ptr<Thisclass>(New());} \
  \
  private: \
  \
    static  Thisclass*              NewInstance(); \
    virtual Thisclass*              MakeClone() const

//static bool                     StaticIsType(const ftk::String& type);
//static bool                     StaticIsType(const ftk::TypeID& type);

#define ftkBaseTypeMacro(className) \
  public: \
    typedef className Rootclass; \
  private: \
    void                            Delete() {delete this;} \
    ftkCommonTypeMacro(className); \
  \
    Rootclass::Pointer              Clone() const {Rootclass* p = MakeClone();return Rootclass::Pointer(p);} \
    enum{}

  //Rootclass::Pointer              NewObjectInstance() const {return Rootclass::Pointer(NewInstance());}

#define ftkTypeMacro(className) \
  public: \
    typedef Thisclass Superclass; \
    \
  private: \
    ftkCommonTypeMacro(className)

#define ftkAbstractTypeMacro(className) \
  ftkTypeMacro(className)

#define ftkCxxCommonTypeMacro(className) \
  const std::type_info&               className::TypeId() {return typeid(className::Thisclass);} \
  const std::string&                  className::TypeName() {static std::string className(#className);return className;} \
  const std::type_info&               className::GetTypeId() const {return TypeId();} \
  const std::string&                  className::GetTypeName() const {return TypeName();} \
  bool                                className::IsA(const std::type_info& type) const {return type == GetTypeId();} \
  bool                                className::IsA(const std::string& type) const {return type == GetTypeName();} \
  \
  className::Thisclass::Pointer       className::SafeDownCast(className::Rootclass::PointerArg o) {try{return dynamic_cast<Thisclass::Pointer>(o);}catch(std::bad_cast){return className::Thisclass::Pointer(nullptr);}} \
  className::Thisclass::ConstPointer  className::SafeDownCast(className::Rootclass::ConstPointerArg o) {try{return dynamic_cast<Thisclass::ConstPointer>(o);}catch(std::bad_cast){return className::Thisclass::ConstPointer(nullptr);}} \
  \
  className::Rootclass::Pointer       className::FactoryNew() {return className::Rootclass::Pointer(NewInstance());} \
  className::Thisclass::Pointer       className::New() {return className::Thisclass::Pointer(NewInstance());} \
  \
  className::Thisclass*               className::MakeClone() const {if(className::Thisclass* obj = NewInstance()){obj->DeepCopy(this);return obj;}return nullptr;} \
  \
  enum{}


  //className::Thisclass*               className::NewInstance() {return new Thisclass();}
  //bool                                className::IsStaticType(const ftk::String& type) {return type == GetStaticTypeName();}
  //bool                                className::IsStaticType(const ftk::TypeID& type) {return type == GetStaticTypeId();}


#define ftkCxxAbstractTypeMacro(className) \
  className::Thisclass* className::NewInstance() {return nullptr;} \
  ftkCxxCommonTypeMacro(className)
  //className::Thisclass::Pointer       className::New() {return nullptr;}

#define ftkCxxTypeMacro(className) \
  className::Thisclass* className::NewInstance() {return new className::Thisclass();} \
  ftkCxxCommonTypeMacro(className)
  //className::Thisclass::Pointer className::New() {className::Thisclass::Pointer obj(new className::Thisclass);return obj;}

//className::Thisclass::Pointer className::SafeNew() {try{return New();}catch(std::bad_alloc){return className::Thisclass::Pointer(nullptr);}} \

namespace ftk
{
	template<class T>
	class Properties;

	class Object
	{
		ftkBaseTypeMacro(Object);
	public:
		Object();

		virtual ~Object();

	protected:
		bool operator==(const Object& obj) const;
		bool Equal(const Object& obj) const;
		Object& DeepCopy(const Object& src);

		virtual bool Equal(const Object* obj) const;
		virtual Object& DeepCopy(const Object* src);
	};
}

namespace ftk
{
	class DataWriter
	{
	public:
		virtual ~DataWriter() = 0 {}
		virtual void StartWriteObject(const std::string& name, const Object& obj) = 0;
		virtual void WriteProperty(const std::string& name, const std::string& value) = 0;
		virtual void StopWriteObject() = 0;
	};

	class DataReader
	{
	public:
		virtual ~DataReader() = 0 {}
		virtual void        StartReadObject(const std::string& name, Object& obj) = 0;
		virtual std::string ReadProperty(const std::string& name) = 0;
		virtual void        StopReadObject() = 0;
	};

	/*class StorableProperty
	{
	public:
		virtual ~StorableProperty() {}
		virtual void Write(const Object& obj, const std::string& name, DataWriter& writer) const = 0;
		virtual void Read(Object& obj, const std::string& name, DataReader& reader) = 0;
	};*/

	template <class T, class U>
	class Conversion
	{
		typedef char Small;
		struct Big
		{
			char dummy[2];
		};
		struct scope
		{
			static Small test(const U&);
			static Big   test(...);
		};
		static const T& t();
	public:
		enum
		{
			EXISTS = sizeof(scope::test(t())) == sizeof(Small)
		};
	};

	template<class Object>
	class Property
	{
	public:
		Property() {}
		virtual ~Property() = default;
		//virtual Property* Copy() = 0;
		virtual std::string GetString(const Object& o) const = 0;

		//virtual void Write(const Object& obj, const std::string& name, DataWriter& writer) const {};
		//virtual void Read(Object& obj, const std::string& name, DataReader& reader) {};
	};

	template<class Object>
	class Properties : public historic_map<std::string, std::unique_ptr<Property<Object> > >
	{
	public:
		Properties(const Properties&) = delete;
		Properties& operator=(const Properties&) = delete;
	protected:
		Properties() = default;
		~Properties() = default;
	private:
		static Properties& init_props()
		{
			static Properties props;
			return props;
		}
	};

	template <bool>
	struct PropertyTraits
	{
	};

	template<>
	struct PropertyTraits<false>
	{
		template <class Field>
		static void Write(const Field& field, const std::string& name, DataWriter& writer)
		{
			std::ostringstream str;
			str << field;
			writer.WriteProperty(name, str.str());
		}
		template <class Field>
		static void Read(Field& field, const std::string& name, DataReader& reader)
		{
			const std::string& value = reader.ReadProperty(name);
			std::istringstream str(value);
			str >> field;
		}
	};
	template <>
	struct PropertyTraits<true>
	{
		static void Write(const Object& field, const std::string& name, DataWriter& writer)
		{
			writer.StartWriteObject(name, field);
//			for (auto& entry : field.GetProperties())
	//			entry.second->Write(field, entry.first, writer);
			writer.StopWriteObject();
		}
		static void Read(Object& field, const std::string& name, DataReader& reader)
		{
			reader.StartReadObject(name, field);
		//	for (auto& entry : field.GetProperties())
			//	entry.second->Read(field, entry.first, reader);
			reader.StopReadObject();
		}
	};

	template <class Object, class Field>
	class PropertyImpl : public Property<Object>
	{
	private:
		friend Object;
		friend Object::InnerProperties;
	public:
		PropertyImpl(Field Object::* field) : m_field(field) {}
		virtual std::string GetString(const Object& o) const
		{
			std::ostringstream str;
			str << o.*m_field;
			return str.str();
		}
	//public:
	protected:
		Field Object::* m_field;
	};

	template <class T, class TField>
	class PropertyImplOld : public Property<T>
	{
		friend T::InnerProperties;
		//public:
		PropertyImplOld(TField T::* field) : m_field(field) {}
	public:
		TField&     Get(const T& t) { return t.*m_field; }
		void        Set(T& t, const TField& tf) { t.*m_field = tf; }
		//std::string GetString(const class Object& o) override { const T& t = dynamic_cast<const T&>(o); std::stringstream strm; strm << t.*m_field; return strm.str(); }

		void Write(const Object& obj, const std::string& name, DataWriter& writer) const override
		{
			PropertyTraits<Conversion<TField, Object>::EXISTS> ::Write(static_cast<const T&>(obj).*m_field, name, writer);
		}
		void Read(Object& obj, const std::string& name, DataReader& reader) override
		{
			//PropertyTraits<Conversion<TField, Object>::EXISTS> ::Read(static_cast<T&>(obj).*m_field, name, reader);
		}
	protected:
		TField T::* m_field;
	};

	template <class T, class TField>
	class PropertyObjImpl : public Property<T>
	{
		friend T::InnerProperties;
		//public:
		PropertyObjImpl(TField T::* field) : m_field(field) {}
	public:
		//std::string GetString(const class Object& o) override { return {}; }

		void Write(const Object& obj, const std::string& name, DataWriter& writer) const override
		{
			PropertyTraits<Conversion<TField, Object>::EXISTS> ::Write(static_cast<const T&>(obj).*m_field, name, writer);
		}
		void Read(Object& obj, const std::string& name, DataReader& reader) override
		{
			//PropertyTraits<Conversion<TField, Object>::EXISTS> ::Read(static_cast<T&>(obj).*m_field, name, reader);
		}
	protected:
		TField T::* m_field;
	};

// 	template <class T, class TField>
// 	class StorablePropertyImpl : public StorableProperty
// 	{
// 	protected:
// 		TField T::* m_field;
// 	public:
// 		StorablePropertyImpl(TField T::* field) : m_field(field) {}
// 		virtual void Write(const Object& obj, const std::string& name, DataWriter& writer) const
// 		{
// 			PropertyTraits<Conversion<TField, Object>::EXISTS> ::Write(static_cast<const T&>(obj).*m_field, name, writer);
// 		}
// 		virtual void Read(Object& obj, const std::string& name, DataReader& reader)
// 		{
// 			PropertyTraits<Conversion<TField, Object>::EXISTS> ::Read(static_cast<T&>(obj).*m_field, name, reader);
// 		}
// 	};
}

#define ftkPropertiesMacro \
  public: \
    class InnerProperties : public ftk::Properties \
    { \
      friend class Thisclass; \
      InnerProperties(const InnerProperties&); \
      InnerProperties& operator=(const InnerProperties&); \
      static InnerProperties& init_props() \
      { \
        static InnerProperties props; \
        return props; \
      } \
      protected: \
        InnerProperties(){} \
    }; \
    virtual ftk::Properties&        GetProperties(){return InnerProperties::init_props();} \
    virtual const ftk::Properties&  GetProperties() const {return InnerProperties::init_props();} \
    virtual void Save(DataWriter& writer) const = 0; \
    virtual void Load(DataReader& reader) = 0; \
  private: \
    enum{}


#define ftkBeginPropertiesMacro \
  public: \
    class InnerProperties : public Superclass::InnerProperties \
    { \
      friend Thisclass; \
      template <class TField> \
      void add_property(const std::string& name, TField Thisclass::* field) \
      { \
        if(ftk::Conversion<TField, Rootclass>::EXISTS) \
          operator[](name) = std::unique_ptr<ftk::PropertyObjImpl<Thisclass, TField> >(new ftk::PropertyObjImpl<Thisclass, TField>(field)); \
        else \
          operator[](name) = std::unique_ptr<ftk::PropertyImpl<Thisclass, TField> >(new ftk::PropertyImpl<Thisclass, TField>(field)); \
      } \
      static InnerProperties& init_props() \
      { \
        static InnerProperties props; \
        return props; \
      } \
    public: \
      InnerProperties() \
      {

#define ftkDefPropertyMacro(prop) \
        add_property(#prop, &Thisclass::m_##prop)

#define ftkEndProperties \
      } \
    }; \
    friend class Thisclass::InnerProperties; \
    virtual ftk::Properties&        GetProperties(){return InnerProperties::init_props();} \
    virtual const ftk::Properties&  GetProperties() const {return InnerProperties::init_props();} \
    void Save(DataWriter& writer) const override \
    { \
       for (auto& prop : GetProperties()) \
         prop.second->Write(*this, prop.first, writer); \
    } \
    void Load(DataReader& reader) override \
    { \
      for (auto& prop : GetProperties()) \
        prop.second->Read(*this, prop.first, reader); \
    } \
  private:


namespace ftk
{
	class ObjectWithProperties : public Object
	{
		ftkTypeMacro(ObjectWithProperties);
		//ftkPropertiesMacro;
		// 		virtual void Store(DataWriter& writer) const
		// 		{
		// 			//for (auto& prop : GetProperties())
		// 			{
		// 				//writer.WriteProperty(prop.)
		// 			}
		// 		}// writer.WriteProperty();
		// 		virtual void Restore(DataReader& reader)
		// 		{
		// 		}
	};
}

namespace ftk
{
	class IPropertyVisitor
	{
	public:
		template<class T>
		void visit(const std::string& name, const T& value)
		{
		}
	};

	class Node : public ObjectWithProperties
	{
		ftkTypeMacro(Node);
	public:
		class InnerProperties : Properties<Node>
		{
			friend class Node;
			template <class TField>
			void add_property(const std::string& name, TField Thisclass::* field)
			{
				operator[](name) = std::make_unique<ftk::PropertyImpl<Thisclass, TField> >(field);
			}
		};
		InnerProperties& init_props()
		{
			static InnerProperties props;
			props.add_property("name", &Node::m_name);
			return props;
		}
		virtual InnerProperties& GetProperties() { static InnerProperties& props = init_props(); return props; }
		//ftkBeginPropertiesMacro
		//	ftkDefPropertyMacro(name);
		//ftkEndProperties
	public:

		Node() {}

		Node(std::string_view name) : m_name(name) {}

		~Node() override {}

		const std::string& getName() const { return m_name; }
		void setName(std::string_view value) { m_name = value; }
		void addChild(std::unique_ptr<Node> child) { m_children.push_back(std::move(child)); }
		const std::vector<std::unique_ptr<Node> >& getChildren() const { return m_children; }
		template<class Visitor>
		void visitProperties(Visitor& v)
		{
			for(auto& prop : GetProperties())
			{
				//v.visit(prop.first, prop.second);
			}
		}
	private:
		std::string m_name;
		std::vector<std::unique_ptr<Node> > m_children;
	};

	class NodeDerived : public Node
	{
		ftkTypeMacro(NodeDerived);
	public:
		class InnerProperties : Properties<NodeDerived>
		{
			friend class NodeDerived;
			template <class TField>
			void add_property(const std::string& name, TField Thisclass::* field)
			{
				operator[](name) = std::make_unique<ftk::PropertyImpl<Thisclass, TField> >(field);
			}
		};
		InnerProperties& init_props()
		{
			static InnerProperties props;
			props.add_property("size", &NodeDerived::m_size);
			return props;
		}
		//InnerProperties& GetProperties() override { static InnerProperties& props = init_props(); return props; }
		template<class Visitor>
		void visitProperties(Visitor& v)
		{
			Superclass::visitProperties(v);
			for (auto& prop : GetProperties())
			{
				//v.visit(prop.first, prop.second);
			}
		}

		//class hhh : public Superclass::InnerProperties
		//{
		//};
		//Superclass::InnerProperties* gettt() { return nullptr; }
		//ftkBeginPropertiesMacro
			//ftkDefPropertyMacro(size);
		//ftkEndProperties
	public:
		NodeDerived() : m_size(5) {}
		NodeDerived(std::string_view name, int size) : Superclass(name), m_size(size) {}
		~NodeDerived() override
		{

		}
		const int getSize() const { return m_size; }
		void setSize(int size) { m_size = size; }
	private:
		int m_size;
	};
}
