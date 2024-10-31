#include "gtest/gtest.h"

#include "SerializerCode.h"

#if TEST_SERIALIZER_CODE()

#include "Serialization/Serializer.h"
#include "Serialization/JsonDeserializer.h"

namespace
{
	const char* SimpleClassJson = R"""(
{
	"boolValue": true,
	"intValue": 10,
	"negativeIntValue": -66,
	"floatValue": 3.14,
	"floatFromIntValue": 9,
	"enumValue": "Bar",
	"string": "some string",
	"dynamicString": "some dynamic string",
	"intArray":
	[
		7,
		14,
		28,
		4
	],
	"dynamicArray": [ 1, 5, 7, 12 ]
}
)""";

	const char* NestedClassJson = R"""(
{
	"outerBool": false,
	"nested": 
	{
		"boolValue": true,
		"intValue": 10,
		"negativeIntValue": -66,
		"floatValue": 3.14,
		"floatFromIntValue": 9,
		"enumValue": "Bar",
		"string": "some string",
		"dynamicString": "some dynamic string",
		"intArray": [
			7,
			14,
			28,
			4
		],
		"dynamicArray": [ 1, 5, 7, 12 ]
	},
	"outerInt": 123456,
	"arrayOfClasses": [
		{
			"mBool": true
		},
		{
			"mBool": true
		},
		{
			"mBool": false
		}
	]
}
)""";

	const char* InvalidJson = "asdf";

	const char* OverflowJson = R"""(
{
	"mU8": 257,
	"mFloat": 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.0,
	"mArray": [ 5, 10, 15, 20, 25, 30 ]
}
)""";

	const char* MetadataClassJson = R"""(
{
	"enableDamage": false,
	"damage": 3.14,
	"mult": 2
}
)""";

	const char* DynamicClassJson = R"""(
{
	"mInstances": [
		{
			"__type__": "DeserializationClass1",
			"mBaseField": true,
			"mInt": 5
		},
		{
			"__type__": "DeserializationClass2",
			"mBaseField": true,
			"mString": "im a string"
		},
		{
			"__type__": "DeserializationClass1",
			"mBaseField": false,
			"mInt": 15
		},
		{
			"__type__": "DeserializationClass1",
			"mBaseField": true,
			"mInt": 25
		}
	]
}
)""";
}

TEST(SerializerTests, JsonDeserializeSimpleClass)
{
	cpprefl::serialization::extensions::JsonDeserializer jsonDeserializer(SimpleClassJson);

	cpprefl::serialization::Deserializer deserializer;
	const auto objectResult = deserializer.Deserialize<DeserializeSimple>(jsonDeserializer);
	EXPECT_TRUE(objectResult.has_value());
	const auto& object = *objectResult;

	EXPECT_EQ(object.boolValue, true);
	EXPECT_EQ(object.intValue, 10);
	EXPECT_EQ(object.negativeIntValue, -66);
	EXPECT_FLOAT_EQ(object.floatValue, 3.14f);
	EXPECT_FLOAT_EQ(object.floatFromIntValue, 9.0f);
	EXPECT_EQ(object.enumValue, DeserializeEnum::Bar);

	EXPECT_STREQ(object.string, "some string");
	EXPECT_STREQ(object.dynamicString, "some dynamic string");

	EXPECT_EQ(object.intArray[0], 7);
	EXPECT_EQ(object.intArray[1], 14);
	EXPECT_EQ(object.intArray[2], 28);
	EXPECT_EQ(object.intArray[3], 4);

	EXPECT_EQ(object.dynamicArray.size(), 4);
	EXPECT_EQ(object.dynamicArray.capacity(), 4);
	EXPECT_EQ(object.dynamicArray[0], 1);
	EXPECT_EQ(object.dynamicArray[1], 5);
	EXPECT_EQ(object.dynamicArray[2], 7);
	EXPECT_EQ(object.dynamicArray[3], 12);
}


TEST(SerializerTests, JsonDeserializeNestedClass)
{
	cpprefl::serialization::extensions::JsonDeserializer jsonDeserializer(NestedClassJson);

	cpprefl::serialization::Deserializer deserializer;
	const auto objectResult = deserializer.Deserialize<DeserializeNested>(jsonDeserializer);
	EXPECT_TRUE(objectResult.has_value());
	const auto& object = *objectResult;

	EXPECT_EQ(object.outerBool, false);
	EXPECT_EQ(object.outerInt, 123456);
	EXPECT_EQ(object.nested.boolValue, true);
	EXPECT_EQ(object.nested.intValue, 10);
	EXPECT_EQ(object.nested.negativeIntValue, -66);
	EXPECT_FLOAT_EQ(object.nested.floatValue, 3.14f);
	EXPECT_FLOAT_EQ(object.nested.floatFromIntValue, 9.0f);
	EXPECT_EQ(object.nested.enumValue, DeserializeEnum::Bar);

	EXPECT_STREQ(object.nested.string, "some string");
	EXPECT_STREQ(object.nested.dynamicString, "some dynamic string");

	EXPECT_EQ(object.nested.intArray[0], 7);
	EXPECT_EQ(object.nested.intArray[1], 14);
	EXPECT_EQ(object.nested.intArray[2], 28);
	EXPECT_EQ(object.nested.intArray[3], 4);

	EXPECT_EQ(object.nested.dynamicArray.size(), 4);
	EXPECT_EQ(object.nested.dynamicArray.capacity(), 4);
	EXPECT_EQ(object.nested.dynamicArray[0], 1);
	EXPECT_EQ(object.nested.dynamicArray[1], 5);
	EXPECT_EQ(object.nested.dynamicArray[2], 7);
	EXPECT_EQ(object.nested.dynamicArray[3], 12);

	EXPECT_EQ(object.arrayOfClasses.size(), 3);
	EXPECT_EQ(object.arrayOfClasses[0].mBool, true);
	EXPECT_EQ(object.arrayOfClasses[1].mBool, true);
	EXPECT_EQ(object.arrayOfClasses[2].mBool, false);
}

TEST(SerializerTests, JsonInvalid)
{
	cpprefl::serialization::extensions::JsonDeserializer jsonDeserializer(InvalidJson);

	cpprefl::serialization::Deserializer deserializer;
	const auto objectResult = deserializer.Deserialize<DeserializeNested>(jsonDeserializer);

	EXPECT_FALSE(objectResult.has_value());
}

TEST(SerializerTests, JsonOverflow)
{
	cpprefl::serialization::extensions::JsonDeserializer jsonDeserializer(OverflowJson);

	cpprefl::serialization::Deserializer deserializer;
	const auto objectResult = deserializer.Deserialize<DeserializeOverflow>(jsonDeserializer);
	EXPECT_TRUE(objectResult.has_value());
	const auto& object = *objectResult;

	EXPECT_EQ(object.mU8, (uint8_t)257);
	EXPECT_EQ(object.mFloat, (float)std::numeric_limits<double>::max());
	EXPECT_EQ(object.mArray[0], 5);
	EXPECT_EQ(object.mArray[1], 10);
	EXPECT_EQ(object.mArray[2], 15);
	EXPECT_EQ(object.mArray[3], 1);// This array is only 3 long, so we shouldn't have deserialized the 4th value.
	EXPECT_EQ(object.mPadding, 1);
}

namespace
{
	class JsonMetadataTestDeserializerExtension : public cpprefl::serialization::IObjectDeserializerExtension
	{
	public:
		// Determines if a field can be deserialized.
		bool CanDeserialize(void* classObject, const cpprefl::ClassInfo& classInfo, const cpprefl::FieldInfo& fieldInfo) override
		{
			// The Name("enable") attribute allows a field to skip deserialization based on another field.
			const auto& enableAttr = fieldInfo.GetAttribute(cpprefl::Name("enable"));
			if (enableAttr != nullptr)
			{
				const auto& otherFieldValue = classInfo.GetFieldValueSafe<bool>(classObject, *enableAttr);
				if (otherFieldValue != nullptr && *otherFieldValue == false)
				{
					return false;
				}
			}

			return true;
		}

		// Modifies an integer value.
		void ModifyValue(void* classObject, const cpprefl::ClassInfo& classInfo, const cpprefl::FieldInfo& fieldInfo, int64_t& value) override
		{
			MultiplyValue<int>(fieldInfo, value);
		}

		// Modifies an integer value.
		void ModifyValue(void* classObject, const cpprefl::ClassInfo& classInfo, const cpprefl::FieldInfo& fieldInfo, uint64_t& value) override
		{
			MultiplyValue<int>(fieldInfo, value);
		}

		// Modifies a double value.
		void ModifyValue(void* classObject, const cpprefl::ClassInfo& classInfo, const cpprefl::FieldInfo& fieldInfo, double& value) override
		{
			MultiplyValue<float>(fieldInfo, value);
		}

	private:
		template <typename T, typename U>
		void MultiplyValue(const cpprefl::FieldInfo& fieldInfo, U& value)
		{
			const auto& multiplierAttr = fieldInfo.GetAttribute(cpprefl::Name("multiplier"));
			if (multiplierAttr != nullptr)
			{
				value *= (T)*multiplierAttr;
			}
		}
	};
}

TEST(SerializerTests, JsonMetadata)
{
	cpprefl::serialization::extensions::JsonDeserializer jsonDeserializer(MetadataClassJson);

	cpprefl::serialization::Deserializer deserializer;

	JsonMetadataTestDeserializerExtension extension;
	deserializer.RegisterExtension(extension);

	const auto objectResult = deserializer.Deserialize<DeserializeMetadata>(jsonDeserializer);
	EXPECT_TRUE(objectResult.has_value());
	const auto& object = *objectResult;

	EXPECT_FLOAT_EQ(object.damage, 0.0f);
	EXPECT_EQ(object.mult, 4);
}

TEST(SerializerTests, JsonDynamic)
{
	cpprefl::serialization::extensions::JsonDeserializer jsonDeserializer(DynamicClassJson);

	cpprefl::serialization::Deserializer deserializer;

	JsonMetadataTestDeserializerExtension extension;
	deserializer.RegisterExtension(extension);

	const auto objectResult = deserializer.Deserialize<DeserializationDynamic>(jsonDeserializer);
	EXPECT_TRUE(objectResult.has_value());
	const auto& object = *objectResult;

	EXPECT_EQ(object.mInstances.size(), 4);

	EXPECT_EQ(((DeserializationClass1*)object.mInstances[0])->mInt, 5);
	EXPECT_EQ(object.mInstances[0]->GetBaseField(), true);

	EXPECT_STREQ(((DeserializationClass2*)object.mInstances[1])->mString, "im a string");
	EXPECT_EQ(object.mInstances[1]->GetBaseField(), true);

	EXPECT_EQ(((DeserializationClass1*)object.mInstances[2])->mInt, 15);
	EXPECT_EQ(object.mInstances[2]->GetBaseField(), false);

	EXPECT_EQ(((DeserializationClass1*)object.mInstances[3])->mInt, 25);
	EXPECT_EQ(object.mInstances[3]->GetBaseField(), true);
}

#endif