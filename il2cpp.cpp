#include "il2cpp.h"


namespace IL2CPP {

	Il2CppDomain* domain;
	std::map<Class*, std::map<Class*, std::vector<Class*>>> generic_cache;

	namespace API {
		#define DO_API(r, n, p) IL2CPP_DECLARATION(n)
		#include IL2CPP_API_H
		#undef DO_API
	}

	void EnumerateClasses(Il2CppClass* klass, void* user_data) {

		Class* my_klass = (Class*)klass;

		EnumClassContext* context = reinterpret_cast<EnumClassContext*>(user_data);

		if (strcmp(klass->namespaze, context->generic_class->namespaze) != 0 ||
			strncmp(klass->name, context->generic_class->name,
				strlen(context->generic_class->name)) != 0) {
			return;
		}

		int arg_count = my_klass->GetGenericArgCount();

		if (arg_count != context->parameters.size()) return;

		for (int32_t i = 0; i < arg_count; i++) {
			auto type_param = my_klass->GetGenericArgAt(i);

			if (type_param != context->parameters[i]) return;
		}

		context->resolved.push_back(my_klass);
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
	
	// Assembly

	Il2CppImage* Assembly::GetImage() {
		return this->image;
	}

	IL2CPP::Namespace* Assembly::GetNamespace(const char* namespace_name) {
		return new IL2CPP::Namespace(this, namespace_name);
	}

	Assembly* Assembly::GetList(size_t* size) {
		return (Assembly*)API::il2cpp_domain_get_assemblies(domain, size);
	}

	// Namespace

	Namespace::Namespace(IL2CPP::Assembly* assembly_, const char* name_) {
		this->assembly = assembly_;
		this->name = name_;
	}

	Class* Namespace::GetClass(const char* class_name) {
		return (IL2CPP::Class*)API::il2cpp_class_from_name(assembly->GetImage(), name, class_name);
	}

	// Class

	Class* Class::GetGeneric(std::vector<Class*> classes) {

		if (generic_cache.count(this)) {
			auto cache = generic_cache[this];

			for (auto const& [generic_class, parameters] : cache)
			{
				if (parameters == classes) return generic_class;
			}
		}

		EnumClassContext context;
		context.generic_class = this;
		context.parameters = classes;

		API::il2cpp_class_for_each(&EnumerateClasses, &context);

		if (context.resolved.empty()) return 0;

		return context.resolved[0];
	}

	bool Class::IsGeneric() {
		return API::il2cpp_class_is_generic(this);
	}

	bool Class::IsInflated() {
		return API::il2cpp_class_is_inflated(this);
	}

	int Class::GetGenericArgCount() {
		if (!klass->generic_class) return 0;

		return klass->generic_class->context.class_inst->type_argc;
	}

	Class* Class::GetGenericArgAt(int index) {
		if (index >= this->generic_class->context.class_inst->type_argc) return 0;

		return (Class*)API::il2cpp_class_from_il2cpp_type(this->generic_class->context.class_inst->type_argv[index]);
	}

	Object* Class::GetObject() {
		return GetType()->GetObject();
	}

	Type* Class::GetType() {
		return (Type*)API::il2cpp_class_get_type(this);
	}

	Type* Class::GetType(const char* field_name) {
		return GetField(field_name)->GetType();
	}

	Method* Class::GetMethod(const char* method_name, int args_count) {
		return (Method*)API::il2cpp_class_get_method_from_name(klass, method_name, args_count);
	};

	Field* Class::GetField(const char* field_name) {
		return (Field*)API::il2cpp_class_get_field_from_name(klass, field_name);
	}

	const char* Class::GetName() {
		return  IL2CPP::API::il2cpp_class_get_name(klass);
	}

	const char* Class::GetNamespaze() {
		return  IL2CPP::API::il2cpp_class_get_namespace(klass);
	}

	const char* Class::GetAssemblyName() {
		return  IL2CPP::API::il2cpp_class_get_assemblyname(klass);
	}

	const Il2CppImage* Class::GetImage() {
		return IL2CPP::API::il2cpp_class_get_image(klass);
	}

	// Field

	const char* Field::GetName() {
		return IL2CPP::API::il2cpp_field_get_name(this);
	}

	Type* Field::GetType() {
		return (Type*)API::il2cpp_field_get_type(this);
	}

	size_t Field::GetOffset() {
		return API::il2cpp_field_get_offset(this);
	}

	// Method

	const char* Method::GetName() {
		return API::il2cpp_method_get_name(this);
	}

	// Type

	Class* Type::GetClass() {
		return (Class*)API::il2cpp_type_get_class_or_element_class(this);
	}

	Object* Type::GetObject() {
		return (Object*)API::il2cpp_type_get_object(this);
	}

	const char* Type::GetName() {
		return API::il2cpp_type_get_name(this);
	}

	// IL2CPP_object

	Class* Object::GetGeneric(std::vector<Class*> classes) {
		return GetClass()->GetGeneric(classes);
	}

	Field* Object::GetField(const char* field_name) {
		return GetClass()->GetField(field_name);
	}

	Class* Object::GetClass() {
		auto klass = (Class*)API::il2cpp_object_get_class(this);

		if (klass->IsGeneric()) {
			return (Class*)klass->generic_class->cached_class;
		}

		return klass;
	}

	Type* Object::GetType(const char* field_name) {
		return GetField(field_name)->GetType();
	}

	// IL2CPP_String

	const size_t String::GetLength() {
		return API::il2cpp_string_length(this);
	}

	const wchar_t* String::GetWChars() {
		return (wchar_t*)API::il2cpp_string_chars(this);
	}

	// IL2CPP

	Assembly* Assembly::Resolve(const char* assembly_name) {
		return (Assembly*)API::il2cpp_domain_assembly_open(domain, assembly_name);
	}

	String* String::NewLen(const char* str) {
		return (String*)API::il2cpp_string_new_len(str, strlen(str));
	}

	String* String::New(const char* str){
		return (String*)API::il2cpp_string_new(str);
	}

	Class* Class::Resolve(const char* assembly_name, const char* namespace_name, const char* class_name) {
		return Assembly::Resolve(assembly_name)->GetNamespace(namespace_name)->GetClass(class_name);
	}

	Field* Field::Resolve(const char* assembly_name, const char* namespace_name, const char* class_name, const char* field_name) {
		return Class::Resolve(assembly_name, namespace_name, class_name)->GetField(field_name);
	}

	Method* Method::Resolve(const char* assembly_name, const char* namespace_name, const char* class_name, const char* method_name, int param_count) {
		return Class::Resolve(assembly_name, namespace_name, class_name)->GetMethod(method_name, param_count);
	}
}
