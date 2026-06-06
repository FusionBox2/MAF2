// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "MainWindow.h"

#include <memory>
#include <fstream>
#include <libjson/json.h>
#include <optional>

#include "ftk/Base/FastBimap.h"
#include "ftk/Base/Log.h"
#include "ftk/Core/AttributeFactory.h"
#include "ftk/Core/Node.h"
#include "ftk/Core/Node0.h"
#include "ftk/Core/Node1.h"
#include "ftk/IO/StorageElement.h"
#include "mafDefines.h"

#include "parse.h"
#include "serialize.h"
#include "jsonrapid.h"
#include "xmlxerces.h"
#include "xmlrapid.h"


#include "tree.hpp"

#include <fstream>
#include <iostream>


//using namespace Tree_AG;

//
// FOR SERIALIZATION:
//   I NEED TO MANUALLY REGISTER THE INSTANTATION OF TEMPLATE
//   INCLUDE THE NEEDED #include 
//
using namespace std;
/*BOOST_CLASS_EXPORT_GUID(Node, "node");
BOOST_CLASS_EXPORT_GUID(Inner< string >, "inner_string");
BOOST_CLASS_EXPORT_GUID(Inner< int >, "inner_int");
BOOST_CLASS_EXPORT_GUID(Inner< double >, "inner_double");
BOOST_CLASS_EXPORT_GUID(Leaf< int >, "leaf_int");
BOOST_CLASS_EXPORT_GUID(Leaf< string >, "leaf_string");
BOOST_CLASS_EXPORT_GUID(Leaf< double >, "leaf_double");*/

//



#ifdef OLD_IO_TEST
namespace utils {

#if (__cpp_lib_void_t >= 201411 && !defined(__clang__)) || defined(DOXYGEN)
	/// @brief std::void_t implementation with workarounds for compiler bugs
	///
	/// @ingroup userver_universal
	template <typename... T>
	using void_t = std::void_t<T...>;
#else

	template <typename... T>
	struct make_void_t {
		using type = void;
	};
	template <typename... T>
	using void_t = typename make_void_t<T...>::type;

#endif

}  // namespace utils

namespace meta {

	namespace impl {

		template <typename Default, typename AlwaysVoid,
			template <typename...> typename Trait, typename... Args>
		struct Detector {
			using value_t = std::false_type;
			using type = Default;
		};

		template <typename Default, template <typename...> typename Trait,
			typename... Args>
		struct Detector<Default, utils::void_t<Trait<Args...>>, Trait, Args...> {
			using value_t = std::true_type;
			using type = Trait<Args...>;
		};

		template <template <typename...> typename Template, typename T>
		struct IsInstantiationOf : std::false_type {};

		template <template <typename...> typename Template, typename... Args>
		struct IsInstantiationOf<Template, Template<Args...>> : std::true_type {};

	}  // namespace impl

	/// @see utils::meta::kIsDetected
	struct NotDetected {};

#if defined(__cpp_concepts) || defined(DOXYGEN)

	/// @brief Checks whether a trait is correct for the given template args
	///
	/// Implements the pre-cpp20-concepts detection idiom.
	///
	/// To use, define a templated type alias (a "trait"), which for some type
	/// either is correct and produces ("detects") some result type,
	/// or is SFINAE-d out. Example:
	///
	/// @code
	/// template <typename T>
	/// using HasValueType = typename T::ValueType;
	/// ...
	/// if constexpr (utils::meta::kIsDetected<HasValueType, T>) { ... }
	/// @endcode
	template <template <typename...> typename Trait, typename... Args>
	concept kIsDetected = requires { typename Trait<Args...>; };

#else

	template <template <typename...> typename Trait, typename... Args>
	inline constexpr bool kIsDetected =
		impl::Detector<NotDetected, void, Trait, Args...>::value_t::value;

#endif

	/// @brief Produces the result type of a trait, or utils::meta::NotDetected if
	/// it's incorrect for the given template args
	/// @see utils::meta::kIsDetected
	template <template <typename...> typename Trait, typename... Args>
	using DetectedType =
		typename impl::Detector<NotDetected, void, Trait, Args...>::type;

	/// @brief Produces the result type of a trait, or @a Default if it's incorrect
	/// for the given template args
	/// @see utils::meta::kIsDetected
	template <typename Default, template <typename...> typename Trait,
		typename... Args>
	using DetectedOr = typename impl::Detector<Default, void, Trait, Args...>::type;

	/// Helps in definitions of traits for utils::meta::kIsDetected
	template <typename T, typename U>
	using ExpectSame = std::enable_if_t<std::is_same_v<T, U>>;

	/// Returns `true` if the type if an instantiation of the specified template.
	template <template <typename...> typename Template, typename T>
	inline constexpr bool kIsInstantiationOf =
		impl::IsInstantiationOf<Template, T>::value;

	/// Returns `true` if the type is a fundamental character type.
	/// `signed char` and `unsigned char` are not character types.
	template <typename T>
	inline constexpr bool kIsCharacter =
		std::is_same_v<T, char> || std::is_same_v<T, wchar_t> ||
		std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>;

	/// Returns `true` if the type is a true integer type (not `*char*` or `bool`)
	/// `signed char` and `unsigned char` are integer types
	template <typename T>
	inline constexpr bool kIsInteger =
		std::is_integral_v<T> && !kIsCharacter<T> && !std::is_same_v<T, bool>;

}  // namespace meta

namespace formats::parse
{
	template <class T>
	struct To {};
}
namespace formats::serialize
{
	template <class T>
	struct To {};
}

namespace formats::common {

	namespace impl {

		/// `kHasX` are only intended for internal diagnostic use!
		///
		/// `formats` doesn't support SFINAE, so e.g. `kHasParse` can return `true`
		/// while a usage of `Parse` will fail to compile.

		template <typename Value, typename T>
		using HasParse = decltype(Parse(std::declval<const Value&>(), parse::To<T>{}));

		template <typename Value, typename T>
		using HasSerialize =
			decltype(Serialize(std::declval<const T&>(), serialize::To<Value>{}));

		template <typename Value, typename T>
		using HasConvert =
			decltype(Convert(std::declval<const Value&>(), parse::To<T>{}));

		template <typename Value>
		using IsFormatValue = typename Value::ParseException;

		template <class Value, class T>
		constexpr inline bool kHasParse = meta::kIsDetected<HasParse, Value, T>;

		template <class Value, class T>
		constexpr inline bool kHasSerialize = meta::kIsDetected<HasSerialize, Value, T>;

		template <class Value, class T>
		constexpr inline bool kHasConvert = meta::kIsDetected<HasConvert, Value, T>;

	}  // namespace impl

	/// Used in `Parse` overloads that are templated on `Value`, avoids clashing
	/// with `Parse` from string
	template <class Value>
	constexpr inline bool kIsFormatValue =
		meta::kIsDetected<impl::IsFormatValue, Value>;

	// Unwraps a transient type - tag types, for which ADL-found `Parse` returns
	// another type, not the type specified in `formats::parse::To`. For example,
	// there can be a `IntegerWithMin<42>` type that checks that the value contains
	// a number `>= 42`, then parses and returns an `int`.
	//
	// For a normal type T, just returns the type T itself.
	template <typename Value, typename T>
	using ParseType = decltype(Parse(std::declval<Value>(), parse::To<T>()));

}  // namespace formats::common

namespace formats::parse {
	namespace impl {

		template <typename T, typename Value>
		void CheckInBounds(const Value& value, T x, T min, T max) {
			if (x < min || x > max) {
				throw Value::ParseException("Value is out of bounds");
// 				throw typename Value::ParseException(
// 					fmt::format("Value of '{}' is out of bounds ({} <= {} <= {})",
// 						value.GetPath(), min, x, max));
			}
		}

		template <typename Value>
		float NarrowToFloat(double x, const Value& value) {
			CheckInBounds<double>(value, x, std::numeric_limits<float>::lowest(),
				std::numeric_limits<float>::max());
			return static_cast<float>(x);
		}

		template <typename Dst, typename Value, typename Src>
		Dst NarrowToInt(Src x, const Value& value) {
			static_assert(
				std::numeric_limits<Src>::min() <= std::numeric_limits<Dst>::min() &&
				std::numeric_limits<Src>::max() >= std::numeric_limits<Dst>::max(),
				"expanding cast requested");

			CheckInBounds<Src>(value, x, std::numeric_limits<Dst>::min(),
				std::numeric_limits<Dst>::max());
			return static_cast<Dst>(x);
		}

	}  // namespace impl

	template <typename Value>
	float Parse(const Value& value, To<float>) {
		return impl::NarrowToFloat(value.template As<double>(), value);
	}

	template <typename Value, typename T>
	std::enable_if_t<common::kIsFormatValue<Value>&& meta::kIsInteger<T>, T> Parse(
		const Value& value, To<T>) {
		using IntT = std::conditional_t<std::is_signed<T>::value, int64_t, uint64_t>;
		return impl::NarrowToInt<T>(value.template As<IntT>(), value);
	}

	template <class Value>
	float Convert(const Value& value, To<float>) {
		return impl::NarrowToFloat(value.template ConvertTo<double>(), value);
	}

	template <typename Value, typename T>
	std::enable_if_t<meta::kIsInteger<T>, T> Convert(const Value& value, To<T>) {
		using IntT = std::conditional_t<std::is_signed<T>::value, int64_t, uint64_t>;
		return impl::NarrowToInt<T>(value.template ConvertTo<IntT>(), value);
	}
}  // namespace formats::parse


namespace formats::parse::json
{
	class Value
	{
	public:
		class ParseException : public std::exception { public: using std::exception::exception; };
		Value(std::istream& is)
		{
			m_instance = std::make_unique<Json::Value>();
			m_value = m_instance.get();
			is >> *m_instance;
		}
		Value(const Json::Value& jvalue) : m_value(&jvalue) {}
		template<typename T>
		auto As() const;

		Value operator[](const std::string& name) const { return Value((*m_value)[name]); }
		Value operator[](int idx) const { return Value((*m_value)[idx]); }
		//private:
		std::unique_ptr<Json::Value> m_instance;
		const Json::Value* m_value;
	};

	template<typename T>
	auto Value::As() const
	{
		return Parse(*this, formats::parse::To<T>{});
	}

	auto Parse(const Value& value, To<std::string>)
	{
		return value.m_value->as<std::string>();
	}

	auto Parse(const Value& value, To<int64_t>)
	{
		return value.m_value->as<int64_t>();
	}

	auto Parse(const Value& value, To<uint64_t>)
	{
		return value.m_value->as<uint64_t>();
	}

	auto Parse(const Value& value, To<double>)
	{
		return value.m_value->as<double>();
	}

}

//template <typename T>


class mafStorageElement;
class mafStorageElementBuilder;
namespace Structure
{
	class Job
	{
	public:
		int Store(mafStorageElementBuilder& element) { return InternalStore(element); }
		int Restore(const mafStorageElement& element) { return InternalRestore(element); }

		Job(int id_ = 0, const std::string& name_ = "") :id(id_), name(name_) {}
		int id;
		std::string name;
		int InternalStore(mafStorageElementBuilder& node)
		{
			mafID idd = id;
			auto root = node[_R("Root")];
			root(_R("Id")).SetValue(idd);
			mafString nm(_R(name.c_str()));
			root(_R("Name")).SetValue(nm);
			auto p = root[_R("ArrayTest")];
			p[0].SetValue(_R("First"));
			p[2].SetValue(_R("Second"));
			return MAF_OK;
		}

		int InternalRestore(const mafStorageElement& node)
		{
			mafID idd = node[_R("Root")](_R("Id")).As<mafID>();
			mafString str = node[_R("Root")](_R("Name")).As<mafString>();
			name = str.toStd();
			id = idd;

			return MAF_OK;
		}
	};
	template <typename V>
	auto Parse(const V& value, formats::parse::To<Job>)
	{
		return Job{ value["Id"].As<int>(), value["Exposures"][0]["HeadA"]["Images"][0]["Name"].As<std::string>() };
	}

}
#endif

class TmpApp : public wxApp
{
public:
	TmpApp()
	{
#ifdef WIN32
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF) | _CRTDBG_LEAK_CHECK_DF);
#endif
		SetAppName("wxApp");
	}

	~TmpApp() override
	{
	}

	bool OnInit() override
	{
		if (!wxApp::OnInit())
			return false;

		auto frame = ftk::CreateFrame();
		frame->Show();

		return true;
		{
			LOG_DEBUG() << "bla";
			constexpr utilities::TrivialBiMap colors = [](auto selector)
				{
					return selector()
						.Case("red", 1 )
						.Case("green", 2);
				};
			auto s1 = colors.TryFind(2);
			auto s2 = colors.TryFind("red");
		}
		if (0)
			{
				/*Inner<std::string> root("root");

				Leaf<std::string>* l = new Leaf<std::string>("leaf test son of root");
				root.add_child(l);

				Inner<std::string>* is = new Inner<std::string>("inner test son of root");
				root.add_child(is);

				Leaf<int>* li = new Leaf<int>(10);
				is->add_child(li);

				Inner<int>* n1 = new Inner<int>(1);
				root.add_child(n1);*/

				//std::cout << std::endl << "******** Depth first visit" << std::endl;
				//depth_first(root);


				std::cout << std::endl << "******** Serializing" << std::endl;

				// create and open a character archive for output
				std::ofstream ofs("D:\\Downloads\\cras\\cras.tst");
				//boost::archive::text_oarchive oa(ofs);
				// write class instance to archive
				//oa << root;

				std::cout << std::endl << "******** Done" << std::endl;

			}
			model::data::AttributeFactory::Initialize();
			{
			//mafXMLWriter writer(_R("MSF"), _R("2.2"));
				//writer.Save(_R("D:\\Downloads\\cras\\crash.msf"));
		}
		{
				auto node = model::data0::Node::Create("");
				io::Reader reader(_R("MSF"), _R("2.2"));
				//reader.Load(_R("D:\\Downloads\\cras\\cras.msf"));
				reader.Load(_R("D:\\Downloads\\Session 1\\sujet4LHP\\sujet4LHP.msf"));
				auto nd = reader.GetRoot()[_R("Root")].As<model::data0::Node>();
				std::unordered_map<model::data0::Node*, size_t> oid;
			for (auto& node : *nd)
			{
				size_t id = oid.size();
				oid.emplace(&node, id);
			}
				auto e1 = reader.GetRoot()[_R("Root")](_R("MaxNodeId")).As<int>();
				auto vx = io::xmlrapid::Value::FromFile("D:\\Downloads\\cras\\cras.msf");
			auto m1 = vx["MSF"]["Root"]("MaxNodeId").As<int>();
			//auto m2 = vx["MSF"]["Root"]["Length"].As<std::string>();
			vx.Store("D:\\Downloads\\cras\\crash.msf");
		}
			/*io::jsonrapid::Value v;
			auto v1 = v[1];
			auto v2 = v["bla"];
			auto e1 = v.As<int>();
			auto e2 = v.As<unsigned>();
			auto e3 = v.As<long>();
			auto e4 = v.As<long long>();
			auto e5 = v.As<unsigned long long>();
			auto e6 = v.As<bool>();
			auto e7 = v.As<double>();
			auto e8 = v.As<float>();
			auto e9 = v.As<std::string>();*/
			//io::rapidjson::ValueBuilder b1(e1);
			//io::rapidjson::ValueBuilder b6(e6);
			//auto ea = v.as<std::exception>();

			io::xmlxerces::ValueBuilder builder;
			builder["MSF"]("Version") = "2.2";
			builder["MSF"]["Root"]["v1"] = (unsigned long)5;
			//builder["MSF"]("Flag") = true;
			builder["MSF"]["Root"]["Length"] = "2.2";
			builder["MSF"]["Root"]["Width"] = 2;
			auto bb = builder["MSF"]["Root"];
		bb["W"][0] = "First";
			builder["MSF"]["Root"]["W"][1] = "Second";
			//builder["MSF"]["Root"] = 2;
			//builder["MSF"]["Root3"] = 2;
			//vx.Store("D:\\Downloads\\cras\\crash.msf");
		  //auto b2 = builder["MSF"]["Root"]["Attributes"]["Item"][0] = "LHDL";
			//builder.extractValue().Store("D:\\Downloads\\cras\\crash.msf");
			builder.extractValue().Store("D:\\Downloads\\cras\\crash.json");
			auto vv = builder.extractValue()["MSF"]("Version").As<std::string>();


			// 		auto p = new uint8_t[1024];
// 		auto newCam = std::make_unique<Structure::Job>();

		{
			//mafXMLParser restore(_R("TST"), _R("1.0"));
			//restore.SetURL(_R("sample.xml"));
			//restore.Restore(newCam.get());
		}



// 		std::ifstream ifs("sample.json");
// 		formats::parse::json::Value value_(ifs);
// 		formats::parse::json::Value id_ = value_["Id"];
// 		formats::parse::json::Value name_ = value_["Name"];
// 		auto id = id_.As<int>();
// 		auto name = name_.As<Json::String>();
// 		auto job = value_.As<Structure::Job>();

		//mafXMLParser restore(_R("TST"), _R("1.0"));
		//restore.SetURL(_R("sample_.xml"));
		//restore.Store(&job);
	}

	int  OnExit() override
	{
		return wxApp::OnExit();
	}
};

wxDECLARE_APP(TmpApp);
wxIMPLEMENT_APP(TmpApp);
