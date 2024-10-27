using ClangSharp.Interop;
using System.Diagnostics.CodeAnalysis;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace CppRefl.CodeGeneration.Reflection
{
	public class TypeHashCollisionException(TypeInfo typeInfo1, TypeInfo typeInfo2) : HashCollisionException($"Types '{typeInfo1}' and '{typeInfo2}' have the same hashes.");
	public class FunctionHashCollisionException(FunctionInfo functionInfo, FunctionInfo functionInfo2) : HashCollisionException($"Functions '{functionInfo}' and '{functionInfo2}' have the same hashes.");
	public class FieldHashCollisionException(ClassInfo classInfo, FieldInfo fieldInfo1, FieldInfo fieldInfo2) : HashCollisionException($"Class fields '{classInfo}::{fieldInfo1}' and '{classInfo}::{fieldInfo2}' have the same hashes.");
	public class MethodHashCollisionException(ClassInfo classInfo, MethodInfo methodInfo1, MethodInfo methodInfo2) : HashCollisionException($"Class methods '{classInfo}::{methodInfo1}' and '{classInfo}::{methodInfo2}' have the same hashes.");

	// TODO: Remove once this project is merged with CppRefl.Compiler.
	internal static class ClangExtensions
	{
		public static bool IsValid(this CXTypeKind kind) => kind != CXTypeKind.CXType_Invalid;
		public static bool IsInvalid(this CXTypeKind kind) => !IsValid(kind);

		public static bool IsValid(this CXType type) => type.kind.IsValid();
		public static bool IsInvalid(this CXType type) => type.kind.IsInvalid();
	}

	/// <summary>
	/// Objects defined in a single file.
	/// </summary>
	public record FileObjects
	{
		/// <summary>
		/// List of reflected classes in the given file.
		/// </summary>
		public IEnumerable<ClassInfo> Classes { get; init; } = Enumerable.Empty<ClassInfo>();

		/// <summary>
		/// List of reflected enums in the given file.
		/// </summary>
		public IEnumerable<EnumInfo> Enums { get; init; } = Enumerable.Empty<EnumInfo>();

		/// <summary>
		/// List of reflected aliases in the given file.
		/// </summary>
		public IEnumerable<AliasInfo> Aliases { get; init; } = Enumerable.Empty<AliasInfo>();

		/// <summary>
		/// List of reflected functions in the given file.
		/// </summary>
		public IEnumerable<FunctionInfo> Functions { get; init; } = Enumerable.Empty<FunctionInfo>();

		/// <summary>
		/// Did this file define any reflected objects?
		/// </summary>
		/// <returns></returns>
		public bool Any() => Classes.Any() || Enums.Any() || Aliases.Any() || Functions.Any();
	}

	/// <summary>
	/// A collection of all the reflected objects in a program.
	/// </summary>
	public class Registry
	{
		/// <summary>
		/// Extension for a serialized registry.
		/// </summary>
		public const string RegistryExtension = ".reflregistry.json";

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
		/// Reflected types.
		/// </summary>
		public IDictionary<string, TypeInfo> Types { get; init; } = new Dictionary<string, TypeInfo>();

		/// <summary>
		/// Lookup table for type hashes.
		/// </summary>
		[JsonIgnore]
		private IDictionary<uint, TypeInfo> TypeHashes { get; } = new Dictionary<uint, TypeInfo>();

		/// <summary>
		/// Lookup table for clang types.
		/// </summary>
		[JsonIgnore]
		private IDictionary<CXType, TypeInfo> ClangTypes { get; } = new Dictionary<CXType, TypeInfo>();

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
		/// Lookup table for function name hashes.
		/// </summary>
		[JsonIgnore]
		private IDictionary<uint, FunctionInfo> FunctionHashes { get; } = new Dictionary<uint, FunctionInfo>();

		/// <summary>
		/// Function used to hash string names.
		/// </summary>
		[JsonIgnore]
		public Func<string, uint> HashFunction { get; init; } = Hash.Crc32;

		/// <summary>
		/// Returns the innermost type of a given type. For example, if the type represents `int*&`, the innermost type is `int`.
		/// </summary>
		/// <param name="typeInfo"></param>
		/// <returns></returns>
		public TypeInfo GetInnermostType(TypeInfo typeInfo)
		{
			CXType innermostType = typeInfo.ClangCursor.Type;
			while (innermostType.PointeeType.IsValid())
			{
				innermostType = innermostType.PointeeType;
			}

			return ClangTypes[innermostType];
		}

		/// <summary>
		/// Try to add an object to a dictionary.
		/// </summary>
		/// <param name="objectInfo"></param>
		/// <param name="objects"></param>
		/// <param name="hashes"></param>
		/// <param name="collidingItem"></param>
		public bool TryAddObject<T>(T objectInfo, IDictionary<string, T> objects, IDictionary<uint, T> hashes, [NotNullWhen(false)] out T? collidingItem) where T : INameMixin
		{
			string name = objectInfo.QualifiedName();

			collidingItem = Hash.FindHashCollision(hashes, name, x => x.QualifiedName(), HashFunction, out var hash);
			if (collidingItem != null)
			{
				return false;
			}

			objects.Add(name, objectInfo);
			hashes.Add(hash, objectInfo);

			return true;
		}

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
			collection.Values.Where(x => x.Metadata.SourceLocation.FileInfo.FullName.StartsWith(module.FullName));


		/// <summary>
		/// Create a reflected type.
		/// </summary>
		/// <param name="typeInfo"></param>
		public TypeInfo AddType(TypeInfo typeInfo)
		{
			if (!TryAddObject(typeInfo, Types, TypeHashes, out var collidingTypeInfo))
			{
				throw new TypeHashCollisionException(typeInfo, collidingTypeInfo);
			}

			// Add the clang type lookup
			if (typeInfo.ClangType.IsValid())
			{
				ClangTypes.Add(typeInfo.ClangType, typeInfo);
			}

			return typeInfo;
		}

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
		public void AddClass(ClassInfo classInfo)
		{
			// Check for hash collisions.
			var collidingFields = Hash.FindHashCollision(classInfo.Fields, x => x.Name, HashFunction);
			if (collidingFields != null)
			{
				throw new FieldHashCollisionException(classInfo, collidingFields.Item1, collidingFields.Item2);
			}

			var collidingMethods = Hash.FindHashCollision(classInfo.Methods, x => x.Name, HashFunction);
			if (collidingMethods != null)
			{
				throw new MethodHashCollisionException(classInfo, collidingMethods.Item1, collidingMethods.Item2);
			}

			Classes.Add(classInfo.Type.QualifiedName(), classInfo);
		}

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
		public FunctionInfo AddFunction(FunctionInfo functionInfo)
		{
			if (!TryAddObject(functionInfo, Functions, FunctionHashes, out var collidingFunctionInfo))
			{
				throw new FunctionHashCollisionException(functionInfo, collidingFunctionInfo);
			}

			return functionInfo;
		}

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

		/// <summary>
		/// Returns all the functions within a module.
		/// </summary>
		/// <param name="moduleDirectory"></param>
		/// <returns></returns>
		public IEnumerable<FunctionInfo> GetFunctionsWithinModule(DirectoryInfo moduleDirectory) =>
			GetObjectsWithinModule(moduleDirectory, Functions);


		/// <summary>
		/// Returns all the objects defined in a file.
		/// </summary>
		/// <param name="file"></param>
		/// <param name="reflectedOnly"></param>
		/// <returns></returns>
		public FileObjects GetObjectsInFile(FileInfo file, bool reflectedOnly = false)
		{
			IEnumerable<T> FindFileObjects<T>(IEnumerable<T> items) where T : ObjectInfo
			{
				return items.Where(x =>
					(!reflectedOnly || x.Metadata.IsReflected) &&
					x.Metadata.SourceLocation.FileInfo.FullName == file.FullName);
			}

			return new FileObjects()
			{
				Classes = FindFileObjects(Classes.Values),
				Enums = FindFileObjects(Enums.Values),
				Aliases = FindFileObjects(Aliases.Values),
				Functions = FindFileObjects(Functions.Values),
			};
		}


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

		/// <summary>
		/// Deserialize a registry from a JSON file.
		/// </summary>
		/// <param name="filename"></param>
		/// <returns></returns>
		public static Registry? FromJsonFile(string filename)
		{
			return FromJson(File.ReadAllText(filename));
		}

		/// <summary>
		/// Reads and combines all individual file registries under a folder.
		/// </summary>
		/// <param name="moduleName"></param>
		/// <param name="moduleDirectory"></param>
		/// <param name="outputDirectory"></param>
		/// <returns></returns>
		public static Registry CollectFileRegistries(string moduleName, DirectoryInfo moduleDirectory, DirectoryInfo outputDirectory)
		{
			Registry moduleRegistry = new();

			// Collect file registries.
			var registryFiles = outputDirectory.EnumerateFiles($"*{RegistryExtension}", SearchOption.AllDirectories);
			foreach (var registryFile in registryFiles)
			{
				// Skip it if this the previously generated module registry.
				if (registryFile.Name == $"{moduleName}{RegistryExtension}")
				{
					continue;
				}

				// Ensure the source file still exists.
				var sourceFile = CodeGenerator.GeneratedToSource(registryFile, moduleDirectory, outputDirectory, ".h");
				if (!sourceFile.Exists)
				{
					// Doesn't exist, don't merge this file in. In fact, let's delete it.
					registryFile.Delete();
					continue;
				}

				// Load the registry.
				var fileRegistry = FromJsonFile(registryFile.FullName);
				if (fileRegistry == null)
				{
					throw new Exception($"Failed to load registry from '{registryFile.FullName}'.");
				}

				// Merge it in.
				moduleRegistry.Merge(fileRegistry);
			}

			return moduleRegistry;
		}
	}
}
