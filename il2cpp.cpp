#include "il2cpp.h"


namespace IL2CPP {

	Il2CppDomain* domain;

	namespace API {
		#define DO_API(r, n, p) IL2CPP_DECLARATION(n)
		#include IL2CPP_API_H
		#undef DO_API
	}

	bool Initialize(void* game_assembly) {

		#define DO_API(r, n, p) IL2CPP_FIND_FUNCTION(n)
		#include IL2CPP_API_H
		#undef DO_API

		domain = API::il2cpp_domain_get();

		return true;
	}

	bool Initialize()
	{
		return Initialize(Memory::FindModule("GameAssembly.dll"));
	}

	void Attach() {
		API::il2cpp_thread_attach(domain);
	}
	
	// ASSEMBLY

	Il2CppImage* ASSEMBLY::Image() {
		return this->image;
	}

	NAMESPACE* ASSEMBLY::Namespace(const char* namespace_name) {
		return new NAMESPACE(this, namespace_name);
	}

	ASSEMBLY* ASSEMBLY::List(size_t* size) {
		return (ASSEMBLY*)IL2CPP::API::il2cpp_domain_get_assemblies(domain, size);
	}

	// NAMESPACE

	NAMESPACE::NAMESPACE(ASSEMBLY* assembly_, const char* name_) {
		assembly = assembly_;
		name = name_;
	}

	CLASS* NAMESPACE::Class(const char* class_name) {
		return (CLASS*)API::il2cpp_class_from_name(assembly->Image(), name, class_name);
	}

	// CLASS

	TYPE* CLASS::Type() {
		return (TYPE*)API::il2cpp_class_get_type(this);
	}

	TYPE* CLASS::Type(const char* field_name) {
		return Field(field_name)->Type();
	}

	METHOD* CLASS::Method(const char* method_name, int args_count) {
		return (METHOD*)API::il2cpp_class_get_method_from_name(klass, method_name, args_count);
	};

	FIELD* CLASS::Field(const char* field_name) {
		return (FIELD*)API::il2cpp_class_get_field_from_name(klass, field_name);
	}

	const char* CLASS::Name() {
		return  IL2CPP::API::il2cpp_class_get_name(klass);
	}

	const char* CLASS::Namespaze() {
		return  IL2CPP::API::il2cpp_class_get_namespace(klass);
	}

	const char* CLASS::AssemblyName() {
		return  IL2CPP::API::il2cpp_class_get_assemblyname(klass);
	}

	const Il2CppImage* CLASS::Image() {
		return IL2CPP::API::il2cpp_class_get_image(klass);
	}

	// FIELD

	const char* FIELD::Name() {
		return IL2CPP::API::il2cpp_field_get_name(this);
	}

	TYPE* FIELD::Type() {
		return (TYPE*)API::il2cpp_field_get_type(this);
	}

	size_t FIELD::Offset() {
		return API::il2cpp_field_get_offset(this);
	}

	// Method

	const char* METHOD::Name() {
		return API::il2cpp_method_get_name(this);
	}

	// Type

	CLASS* TYPE::Class() {
		return (CLASS*)API::il2cpp_type_get_class_or_element_class(this);
	}

	OBJECT* TYPE::Object() {
		return (OBJECT*)API::il2cpp_type_get_object(this);
	}

	const char* TYPE::Name() {
		return API::il2cpp_type_get_name(this);
	}

	// IL2CPP_OBJECT

	FIELD* OBJECT::Field(const char* field_name) {
		return Class()->Field(field_name);
	}

	CLASS* OBJECT::Class() {
		return (CLASS*)API::il2cpp_object_get_class(this);
	}

	TYPE* OBJECT::Type(const char* field_name) {
		return Field(field_name)->Type();
	}

	// IL2CPP_STRING

	const size_t STRING::Length() {
		return API::il2cpp_string_length(this);
	}

	const wchar_t* STRING::WChars() {
		return (wchar_t*)API::il2cpp_string_chars(this);
	}

	// IL2CPP

	ASSEMBLY* Assembly(const char* assembly_name) {
		return (ASSEMBLY*)API::il2cpp_domain_assembly_open(domain, assembly_name);
	}

	STRING* String(const char* str){
		return (STRING*)API::il2cpp_string_new(str);
	}

	CLASS* Class(const char* assembly_name, const char* namespace_name, const char* class_name) {
		return Assembly(assembly_name)->Namespace(namespace_name)->Class(class_name);
	}

	FIELD* Field(const char* assembly_name, const char* namespace_name, const char* class_name, const char* field_name) {
		return Class(assembly_name, namespace_name, class_name)->Field(field_name);
	}

	METHOD* Method(const char* assembly_name, const char* namespace_name, const char* class_name, const char* method_name, int param_count) {
		return Class(assembly_name, namespace_name, class_name)->Method(method_name, param_count);
	}
}
