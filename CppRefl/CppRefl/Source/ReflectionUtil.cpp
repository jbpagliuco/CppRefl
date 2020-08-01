#include "ReflectionUtil.h"

namespace refl
{
	namespace util
	{
		static std::string SerializeInternal(const Registry& reflRegistry, const Class& reflClass, void* data, int indent);

		static std::string BuildIndentString(int indent)
		{
			std::string s = "";
			for (int i = 0; i < indent; ++i) {
				s += "\t";
			}

			return s;
		}

		static std::string SerializePrimitiveFieldValue(const Field& reflField, void* data)
		{
			#define PRIMITIVE_TO_STRING(type) std::to_string(*reflField.GetDataPtr<type>(data))

			switch (reflField.mType)
			{
			case Type::BOOL:
				return reflField.GetDataPtr<bool>(data) ? "true" : "false";

			case Type::UINT8:
				return PRIMITIVE_TO_STRING(uint8_t);
			case Type::INT8:
				return PRIMITIVE_TO_STRING(int8_t);

			case Type::UINT16:
				return PRIMITIVE_TO_STRING(uint16_t);
			case Type::INT16:
				return PRIMITIVE_TO_STRING(int16_t);

			case Type::UINT32:
				return PRIMITIVE_TO_STRING(uint32_t);
			case Type::INT32:
				return PRIMITIVE_TO_STRING(int32_t);

			case Type::UINT64:
				return PRIMITIVE_TO_STRING(uint64_t);
			case Type::INT64:
				return PRIMITIVE_TO_STRING(int64_t);

			case Type::FLOAT:
				return PRIMITIVE_TO_STRING(float);
			case Type::DOUBLE:
				return PRIMITIVE_TO_STRING(double);
			case Type::LONG_DOUBLE:
				return PRIMITIVE_TO_STRING(long double);

			default:
				return "[UNKNOWN DATA TYPE]";
			}

			#undef PRIMITIVE_TO_STRING
		}

		static std::string SerializeField(const Registry& reflRegistry, const Field& reflField, void* data, int indent)
		{
			// String?
			if (reflField.IsString()) {
				return "\"" + *reflField.GetString(data) + "\"";
			}

			// Primitive type?
			if (reflField.IsPrimitive()) {
				return SerializePrimitiveFieldValue(reflField, data);
			}

			// Enum type?
			if (reflField.IsEnumType()) {
				const int * const enumValue = reflField.GetDataPtr<int>(data);
				const std::string enumString = reflRegistry.GetEnum(reflField.mEnumType).GetValueString(*enumValue, true);
				return enumString;
			}

			// Class type?
			if (reflField.IsClassType()) {
				const Class& reflClass = reflRegistry.GetClass(reflField.mClassType);

				std::string output = reflClass.mQualifiedName + "{\n";
				output += SerializeInternal(reflRegistry, reflClass, reflField.GetRawDataPtr(data), indent);
				output += BuildIndentString(indent) + "}";

				return output;
			}

			return "[UNKNOWN]";
		}


		static std::string SerializeInternal(const Registry& reflRegistry, const Class& reflClass, void* data, int indent)
		{
			std::string output = "";

			const std::string fieldLevelIndent = BuildIndentString(indent + 1);
			for (auto field : reflClass.mFields)
			{
				// Indent
				output += fieldLevelIndent;

				// Name
				output += field.mName;

				// Value
				output += " = " + SerializeField(reflRegistry, field, data, indent + 1);

				output += "\n";
			}

			return output;
		}

		std::string Serialize(const Registry& reflRegistry, const Class& reflClass, void* data)
		{
			std::string output = reflClass.mQualifiedName + "{\n";
			output += SerializeInternal(reflRegistry, reflClass, data, 0);
			output += "}";

			return output;
		}
	}
}