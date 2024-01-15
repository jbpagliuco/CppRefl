using System;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace CppRefl.CodeGeneration.Reflection
{
	public class Registry
	{
		/// <summary>
		/// Reflected types.
		/// </summary>
		public IDictionary<string, TypeInfo> Types { get; init; } = new Dictionary<string, TypeInfo>();

		/// <summary>
		/// Reflected classes.
		/// </summary>
		public IDictionary<string, ClassInfo> Classes { get; init; } = new Dictionary<string, ClassInfo>();

		/// <summary>
		/// Reflected enums.
		/// </summary>
		public IDictionary<string, EnumInfo> Enums { get; init; } = new Dictionary<string, EnumInfo>();

		/// <summary>
		/// Reflected aliases.
		/// </summary>
		public IDictionary<string, AliasInfo> Aliases { get; init; } = new Dictionary<string, AliasInfo>();

		/// <summary>
		/// Reflected functions.
		/// </summary>
		public IDictionary<string, FunctionInfo> Functions { get; init; } = new Dictionary<string, FunctionInfo>();


		/// <summary>
		/// Returns a type found in a collection, otherwise returns null.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="name"></param>
		/// <param name="collection"></param>
		/// <returns></returns>
		private T? GetValue<T>(string name, IDictionary<string, T> collection)
		{
			if (collection.TryGetValue(name, out T? value))
			{
				return value;
			}

			return default(T);
		}

		/// <summary>
		/// Returns all the objects within a module.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="module"></param>
		/// <param name="collection"></param>
		/// <returns></returns>
		private IEnumerable<T> GetObjectsWithinModule<T>(DirectoryInfo module, IDictionary<string, T> collection) where T : ObjectInfo => 
			collection.Values.Where(x => x.Metadata.SourceLocation.Filepath.StartsWith(module.FullName));



		/// <summary>
		/// Create a reflected type.
		/// </summary>
		/// <param name="typeInfo"></param>
		public void AddType(TypeInfo typeInfo) => Types.Add(typeInfo.QualifiedName(), typeInfo);

		/// <summary>
		/// Returns a type in this registry.
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		public TypeInfo? GetType(string name) => GetValue(name, Types);
		



		/// <summary>
		/// Add a class to this registry.
		/// </summary>
		/// <param name="classInfo"></param>
		public void AddClass(ClassInfo classInfo) => Classes.Add(classInfo.Type.QualifiedName(), classInfo);

		/// <summary>
		/// Returns a class in this registry.
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		public ClassInfo? GetClass(string name) => GetValue(name, Classes);

		/// <summary>
		/// Returns all classes in this registry.
		/// </summary>
		/// <returns></returns>
		public ICollection<ClassInfo> GetClasses() => Classes.Values;

		/// <summary>
		/// Returns all the classes within a module.
		/// </summary>
		/// <param name="moduleDirectory"></param>
		/// <returns></returns>
		public IEnumerable<ClassInfo> GetClassesWithinModule(DirectoryInfo moduleDirectory) =>
			GetObjectsWithinModule(moduleDirectory, Classes);




		/// <summary>
		/// Add an enum to this registry.
		/// </summary>
		/// <param name="enumInfo"></param>
		public void AddEnum(EnumInfo enumInfo) => Enums.Add(enumInfo.Type.QualifiedName(), enumInfo);

		/// <summary>
		/// Returns an enum in this registry.
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		public EnumInfo? GetEnum(string name) => GetValue(name, Enums);

		/// <summary>
		/// Returns all enums in this registry.
		/// </summary>
		/// <returns></returns>
		public ICollection<EnumInfo> GetEnums() => Enums.Values;

		/// <summary>
		/// Returns all the enums within a module.
		/// </summary>
		/// <param name="moduleDirectory"></param>
		/// <returns></returns>
		public IEnumerable<EnumInfo> GetEnumsWithinModule(DirectoryInfo moduleDirectory) =>
			GetObjectsWithinModule(moduleDirectory, Enums);




		/// <summary>
		/// Add an alias to this registry.
		/// </summary>
		/// <param name="aliasInfo"></param>
		public void AddAlias(AliasInfo aliasInfo) => Aliases.Add(aliasInfo.Type.QualifiedName(), aliasInfo);

		/// <summary>
		/// Returns an alias in this registry.
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		public AliasInfo? GetAlias(string name) => GetValue(name, Aliases);

		/// <summary>
		/// Returns all aliases in this registry.
		/// </summary>
		/// <returns></returns>
		public ICollection<AliasInfo> GetAliases() => Aliases.Values;




		/// <summary>
		/// Add an function to this registry.
		/// </summary>
		/// <param name="functionInfo"></param>
		public void AddFunction(FunctionInfo functionInfo) => Functions.Add(functionInfo.QualifiedName(), functionInfo);

		/// <summary>
		/// Returns an function in this registry.
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		public FunctionInfo? GetFunction(string name) => GetValue(name, Functions);

		/// <summary>
		/// Returns all functions in this registry.
		/// </summary>
		/// <returns></returns>
		public ICollection<FunctionInfo> GetFunctions() => Functions.Values;



		private void MergeDictionary<T>(IDictionary<string, T> baseDictionary, IDictionary<string, T> newDictionary)
		{
			foreach (var it in newDictionary)
			{
				baseDictionary[it.Key] = it.Value;
			}
		}

		/// <summary>
		/// Merge a registry into this registry.
		/// </summary>
		/// <param name="newRegistry"></param>
		public void Merge(Registry newRegistry)
		{
			MergeDictionary(Types, newRegistry.Types);
			MergeDictionary(Classes, newRegistry.Classes);
			MergeDictionary(Enums, newRegistry.Enums);
			MergeDictionary(Aliases, newRegistry.Aliases);
			MergeDictionary(Functions, newRegistry.Functions);
		}



		/// <summary>
		/// Json serialization options.
		/// </summary>
		private static readonly JsonSerializerOptions JsonSerializerOptions = new()
		{
			WriteIndented = true,
			PropertyNameCaseInsensitive = true,
			Converters =
			{
				new JsonStringEnumConverter()
			},
			MaxDepth = 100
		};

		/// <summary>
		/// Convert this registry to JSON.
		/// </summary>
		public string ToJson()
		{
			return JsonSerializer.Serialize(this, JsonSerializerOptions);
		}

		/// <summary>
		/// Convert this registry to JSON.
		/// </summary>
		public void ToJson(Stream stream)
		{
			JsonSerializer.Serialize(stream, this, JsonSerializerOptions);
		}

		/// <summary>
		/// Deserialize a registry from JSON.
		/// </summary>
		/// <param name="json"></param>
		/// <returns></returns>
		public static Registry? FromJson(string json)
		{
			return JsonSerializer.Deserialize<Registry>(json, JsonSerializerOptions);
		}
	}
}
