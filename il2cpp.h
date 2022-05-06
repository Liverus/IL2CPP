#pragma once

#include "il2cpp_types.h"

#define IL2CPP_API_H                "il2cpp_api.h"
#define IL2CPP_TYPEDEF(r, n, p)      typedef r(__cdecl* n ## _t)p;
#define IL2CPP_EXTERN_DECLARATION(n) extern n ## _t n;
#define IL2CPP_DECLARATION(n)        n ## _t n;
#define IL2CPP_FIND_FUNCTION(n)      API:: ## n = Memory::FindFunction<n ## _t>(game_assembly, #n);
// #define IL2CPP_FIND_FUNCTION(n)      API:: ## n = Memory::CopyFunction<n ## _t>(Memory::FindFunction<n ## _t>(game_assembly, #n));

#define DO_API(r, n, p) IL2CPP_TYPEDEF(r, n, p);
#include IL2CPP_API_H
#undef DO_API

#include "hook.h"

namespace IL2CPP
{
	extern Il2CppDomain* domain;

	bool Initialize(void* mod);
	bool Initialize();
	void Attach();

	namespace API {
		#define DO_API(r, n, p) IL2CPP_EXTERN_DECLARATION(n)
		#include IL2CPP_API_H
		#undef DO_API
	}

	struct METHOD;
	struct STRING;
	struct ASSEMBLY;
	struct CLASS;
	struct FIELD;
	struct NAMESPACE;

	template <class T>
	struct ARRAY;
	struct STRING;
	struct OBJECT;
	struct TYPE;

	struct METHOD : public MethodInfo {
		const char* Name();



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

	struct STRING : public Il2CppString {
		const size_t Length();
		const wchar_t* WChars();
	};

	struct ASSEMBLY : public Il2CppAssembly {
		NAMESPACE* Namespace(const char* assembly_name);

		Il2CppImage* Image();
		ASSEMBLY* List(size_t* size);
	};

	struct NAMESPACE {
		NAMESPACE(ASSEMBLY* assembly_, const char* name_);

		CLASS* Class(const char* class_name);

		ASSEMBLY* assembly;
		const char* name;
	};

	struct CLASS : public Il2CppClass {
		TYPE* Type(const char* field_name);
		TYPE* Type();

		// Create Object
		template <class T>
		T New() {
			return API::il2cpp_object_new(this);
		}

		METHOD* Method(const char* method_name, int args_count = 0);

		FIELD* Field(const char* field_name);

		const char* Name();
		const char* Namespaze();
		const char* AssemblyName();
		const Il2CppImage* Image();
	};

	struct FIELD : public FieldInfo {
		const char* Name();
		size_t Offset();

		TYPE* Type();

		template <class T>
		void SetValue(OBJECT* obj, void* value) {
			API::il2cpp_field_set_value(obj, this, value);
		};

		template <class T>
		void SetStaticValue(void* value) {
			API::il2cpp_field_static_set_value(this, value);
		};

		template <class T>
		T GetValue(OBJECT* obj) {

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

	struct TYPE : public Il2CppType {
		CLASS* Class();
		const char* Name();
		OBJECT* Object();
	};

	struct OBJECT : public Il2CppObject {
		FIELD* Field(const char* field_name);

		template <class T>
		T GetValue(const char* field_name) {
			return Field(field_name)->GetValue<T>(this);
		};

		template <class T>
		void SetValue(const char* field_name, void* value) {
			Field(field_name)->SetValue<T>(this, value);
		}

		CLASS* Class();
		TYPE* Type(const char* field_name);
	};

	template <class T>
	struct ARRAY : public Il2CppArray {
		size_t MaxLength() {
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

	ASSEMBLY* Assembly(const char* name);
	STRING* String(const char* str);

	CLASS*  Class(const char* assembly_name, const char* namespace_name, const char* class_name);
	FIELD*  Field(const char* assembly_name, const char* namespace_name, const char* class_name, const char* field_name);
	METHOD* Method(const char* assembly_name, const char* namespace_name, const char* class_name, const char* method_name, int param_count);

	template<class T>
	T Function(const char* assembly_name, const char* namespace_name, const char* klass_name, const char* method_name, int param_count) {
		return Method(assembly_name, namespace_name, klass_name, method_name, param_count)->FindFunction<T>();
	}
};