#pragma once

#include <vector>
#include <map>

#include "il2cpp_types.h"

#define IL2CPP_API_H                "il2cpp_api.h"
#define IL2CPP_TypeDEF(r, n, p)      typedef r(__cdecl* n ## _t)p;
#define IL2CPP_EXTERN_DECLARATION(n) extern n ## _t n;
#define IL2CPP_DECLARATION(n)        n ## _t n;
#define IL2CPP_FIND_FUNCTION(n)      API:: ## n = Memory::FindFunction<n ## _t>(game_assembly, #n);
// #define IL2CPP_FIND_FUNCTION(n)      API:: ## n = Memory::CopyFunction<n ## _t>(Memory::FindFunction<n ## _t>(game_assembly, #n));

#define DO_API(r, n, p) IL2CPP_TypeDEF(r, n, p);
#include IL2CPP_API_H
#undef DO_API

#include "hook.h"

namespace IL2CPP
{
	bool Initialize(void* mod);
	bool Initialize();
	void Attach();

	namespace API {
		#define DO_API(r, n, p) IL2CPP_EXTERN_DECLARATION(n)
		#include IL2CPP_API_H
		#undef DO_API
	}

	struct Method;
	struct String;
	struct Assembly;
	struct Class;
	struct Field;
	struct Namespace;

	template <class T>
	struct Array;
	struct String;
	struct Object;
	struct Type;

	extern std::map<Class*, std::map<Class*, std::vector<Class*>>> generic_cache; // BaseClass: {GenericClass: ArgumentClass[]}
	extern Il2CppDomain* domain;

	struct EnumClassContext {
		Class* generic_class;
		std::vector<Class*> parameters;
		std::vector<Class*> resolved;
	};

	void EnumerateClasses(Class* klass, void* user_data);

	struct Method : public MethodInfo {
		static Method* Resolve(const char* assembly_name, const char* namespace_name, const char* class_name, const char* method_name, int param_count);

		const char* GetName();

		template <class T>
		Memory::Hook* Hook(void* new_method, T* original) {
			auto new_hook = Hook<T>(new_method);
			*original = new_hook->GetOriginal<T>();

			return new_hook;
		}

		template <class T>
		Memory::Hook* Hook(void* new_method) {
			auto new_hook = new Memory::Hook(this->FindFunction<T>(), new_method);

			return new_hook;
		}

		template <class T>
		T FindFunction() {
			return (T)this->methodPointer;
		}
	};

	struct String : public Il2CppString {
		static String* New(const char* string);
		static String* NewLen(const char* string);

		const size_t GetLength();
		const wchar_t* GetWChars();
	};

	struct Assembly : public Il2CppAssembly {
		static Assembly* Resolve(const char* assembly_name);

		Namespace* GetNamespace(const char* assembly_name);

		Il2CppImage* GetImage();
		Assembly* GetList(size_t* size);
	};

	struct Namespace {
		Namespace(Assembly* Assembly_, const char* name_);

		Class* GetClass(const char* class_name);

		Assembly* assembly;
		const char* name;
	};

	struct Class : public Il2CppClass {
		static Class* Resolve(const char* assembly_name, const char* namespace_name, const char* class_name);

		// Generic
		int    GetGenericArgCount();
		Class* GetGenericArgAt(int index);
		Class* GetGeneric(std::vector<Class*> classes);

		Object* GetObject();

		Type* GetType(const char* Field_name);
		Type* GetType();

		Method* GetMethod(const char* method_name, int args_count = 0);

		Field* GetField(const char* Field_name);

		const char* GetName();
		const char* GetNamespaze();
		const char* GetAssemblyName();
		const Il2CppImage* GetImage();

		bool IsGeneric();
		bool IsInflated();
	};

	struct Field : public FieldInfo {
		static Field* Resolve(const char* assembly_name, const char* namespace_name, const char* class_name, const char* Field_name);

		const char* GetName();
		size_t GetOffset();

		Type* GetType();

		template <class T>
		void SetValue(Object* obj, void* value) {
			API::il2cpp_field_set_value(obj, this, value);
		};

		template <class T>
		void SetStaticValue(void* value) {
			API::il2cpp_field_static_set_value(this, value);
		};

		template <class T>
		T GetValue(Object* obj) {

			T ret;
			API::il2cpp_field_get_value(obj, this, &ret);

			return ret;
		};

		template <class T>
		T GetStaticValue() {

			T ret;

			API::il2cpp_field_static_get_value(this, &ret);

			return ret;
		};
	};

	struct Type : public Il2CppType {
		Class* GetClass();
		const char* GetName();
		Object* GetObject();
	};

	struct Object : public Il2CppObject {
		Field* GetField(const char* Field_name);

		Class* GetGeneric(std::vector<Class*> classes);

		// Create Object
		template <class T>
		static T New(Class* klass) {
			return (T)API::il2cpp_object_new(klass);
		}

		 template <class T>
		 static T New(const char* assembly_name, const char* namespace_name, const char* class_name) {
			 return New<T>(Class::Resolve(assembly_name, namespace_name, class_name));
		 }

		template <class T>
		T GetValue(const char* Field_name) {
			return GetField(Field_name)->GetValue<T>(this);
		};

		template <class T>
		void SetValue(const char* Field_name, void* value) {
			GetField(Field_name)->SetValue<T>(this, value);
		}

		Class* GetClass();
		Type* GetType(const char* Field_name);
	};

	template <class T>
	struct Array : public Il2CppArray {
		size_t GetMaxLength() {
			return this->max_length;
		}

		T* GetArrayPointer() {
			return (T*)((ptr_t)this + sizeof(Il2CppArray));
		}

		T& operator[](int& id) {
			return GetValue(id);
		}

		T GetValue(size_t id) {
			return GetArrayPointer()[id];
		}
	};

	template<class T>
	T Function(const char* assembly_name, const char* namespace_name, const char* klass_name, const char* method_name, int param_count) {
		return Method::Resolve(assembly_name, namespace_name, klass_name, method_name, param_count)->FindFunction<T>();
	}
};