using System.Linq;
using System.Text;
using ClangSharp;
using ClangSharp.Interop;
using CppRefl.CodeGeneration;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler
{
	internal static class ClangUtils
	{
		/// <summary>
		/// Format a diagnostic error message.
		/// </summary>
		/// <param name="diagnostic"></param>
		/// <returns></returns>
		public static string FormatDiagnosticMessage(CXDiagnostic diagnostic)
		{
			diagnostic.Location.GetSpellingLocation(out var file, out var line, out var _, out _);
			return $"{file.Name}({line}): {diagnostic}";
		}

		/// <summary>
		/// Create a translation unit from our given parameters.
		/// </summary>
		/// <returns></returns>
		/// <exception cref="AggregateException"></exception>
		public static CXTranslationUnit CreateTranslationUnit(CXIndex index, string sourceFile, IEnumerable<string> clangArgs, bool raiseWarnings, bool raiseErrors)
		{
			if (!File.Exists(sourceFile))
			{
				throw new FileNotFoundException("Entrypoint file not found.", sourceFile);
			}

			var translationUnit = CXTranslationUnit.CreateFromSourceFile(index, sourceFile, clangArgs.ToArray(), ReadOnlySpan<CXUnsavedFile>.Empty);

			var exceptions = new List<Exception>();
			foreach (var diagnostic in translationUnit.DiagnosticSet.Take(100))
			{
				switch (diagnostic.Severity)
				{
					case CXDiagnosticSeverity.CXDiagnostic_Ignored:
					case CXDiagnosticSeverity.CXDiagnostic_Note:
						break;

					case CXDiagnosticSeverity.CXDiagnostic_Warning:
						if (raiseWarnings)
						{
							exceptions.Add(new(FormatDiagnosticMessage(diagnostic)));
						}
						break;

					case CXDiagnosticSeverity.CXDiagnostic_Error:
						if (raiseErrors)
						{
							exceptions.Add(new(FormatDiagnosticMessage(diagnostic)));
						}
						break;

					case CXDiagnosticSeverity.CXDiagnostic_Fatal:
						exceptions.Add(new(FormatDiagnosticMessage(diagnostic)));
						break;
				}
			}

			if (exceptions.Any())
			{
				throw new AggregateException(exceptions);
			}

			if (translationUnit.Cursor.IsInvalid)
			{
				throw new Exception("Failed to create translation unit.");
			}

			return translationUnit;
		}

		/// <summary>
		/// Reflect the children of a cursor.
		/// </summary>
		/// <param name="cursor"></param>
		/// <param name="visitor"></param>
		public static unsafe void VisitCursorChildren(CXCursor cursor, Func<CXCursor, CXChildVisitResult> visitor)
		{
			CXChildVisitResult Visitor(CXCursor child, CXCursor parent, void* clientData) => visitor(child);
			cursor.VisitChildren(Visitor, new CXClientData());
		}

		/// <summary>
		/// Determines if a cursor is reflected and collects all metadata.
		/// </summary>
		/// <param name="cursor"></param>
		/// <param name="metadata"></param>
		private static void GetMetadataInfo(CXCursor cursor, out MetadataInfo metadata)
		{
			bool isReflected = false;
			var tags = new List<string>();
			var attributes = new Dictionary<string, string>();

			CXChildVisitResult Visitor(CXCursor child)
			{
				if (child.Kind == CXCursorKind.CXCursor_AnnotateAttr)
				{
					string annotation = child.Spelling.ToString();
					if (annotation.Contains(CppDefines.Annotation))
					{
						isReflected = true;

						// Attributes are split by a special separator. e.g.:
						// annotate(cpprefl)
						// annotate(cpprefl,just_a_tag)
						// annotate(cpprefl,tag_with_a_value,the_value)
						var split = annotation.Split(CppDefines.AnnotationSeparator, 3);
						if (split.Length == 2)
						{
							tags.Add(split[1]);
						}
						else if (split.Length == 3)
						{
							attributes.Add(split[1], split[2]);
						}
					}
				}

				return CXChildVisitResult.CXChildVisit_Continue;
			}

			VisitCursorChildren(cursor, Visitor);

			metadata = new MetadataInfo()
			{
				IsReflected = isReflected,
				Tags = tags,
				Attributes = attributes,
				SourceLocation = GetSourceLocation(cursor)
			};
		}

		/// <summary>
		/// Collect all annotations on a reflected cursor.
		/// </summary>
		/// <param name="cursor"></param>
		/// <returns></returns>
		public static MetadataInfo? GetReflectedCursorMetadata(CXCursor cursor)
		{
			GetMetadataInfo(cursor, out var metadata);
			return metadata.IsReflected ? metadata : null;
		}

		/// <summary>
		/// Returns metadata about a cursor, even if it is not reflected.
		/// </summary>
		/// <param name="cursor"></param>
		/// <returns></returns>
		public static MetadataInfo GetCursorMetadata(CXCursor cursor)
		{
			GetMetadataInfo(cursor, out var metadata);
			return metadata;
		}

		/// <summary>
		/// Returns the fully qualified name (including namespaces) of a cursor. Strips the :: from global namespace.
		/// </summary>
		/// <param name="cursor"></param>
		/// <returns></returns>
		public static string GetFullyQualifiedName(CXCursor cursor)
		{
			return cursor.ToString();
		}

		/// <summary>
		/// Returns the fully qualified name (including namespaces) of a cursor. Strips the :: from global namespace.
		/// </summary>
		/// <param name="cursor"></param>
		/// <returns></returns>
		public static string GetFullyQualifiedType(CXCursor cursor)
		{
			if (cursor.Type.kind != CXTypeKind.CXType_Invalid)
			{
				return cursor.Type.ToString();
			}

			if (cursor.IsTemplated)
			{
				return cursor.TemplatedDecl.ToString();
			}
			
			throw new NotImplementedException("Unhandled edge case.");
		}

		/// <summary>
		/// Returns the fully qualified name (including namespaces) of a cursor. Strips the :: from global namespace.
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		public static string GetFullyQualifiedType(CXType type)
		{
			return type.ToString();
		}

		/// <summary>
		/// Returns all the template arguments of a type.
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		public static IList<CXType> GetTemplateArguments(CXType type)
		{
			var templateArguments = new List<CXType>();
			for (uint i = 0; i < type.NumTemplateArguments; ++i)
			{
				var templateArg = type.GetTemplateArgument(i);
				templateArguments.Add(templateArg.AsType);
			}

			return templateArguments;
		}

		/// <summary>
		/// Returns all the template arguments of a cursor.
		/// </summary>
		/// <param name="cursor"></param>
		/// <returns></returns>
		public static IList<CXCursor> GetTemplateArguments(CXCursor cursor)
		{
			var templateCursors = new List<CXCursor>();
			for (uint i = 0; i < cursor.GetNumTemplateParameters(0); ++i)
			{
				var templateArg = cursor.GetTemplateParameter(0, i);
				templateCursors.Add(templateArg);
			}

			return templateCursors;
		}

		/// <summary>
		/// Returns the fully qualified name (including namespaces) of a cursor. Strips the :: from global namespace.
		/// </summary>
		/// <param name="cursor"></param>
		/// <returns></returns>
		public static string GetNamespace(CXCursor cursor)
		{
			var cursors = new List<CXCursor>();
			while (cursor.Kind != CXCursorKind.CXCursor_FirstInvalid && cursor.Kind != CXCursorKind.CXCursor_TranslationUnit)
			{
				if (cursor.Kind == CXCursorKind.CXCursor_Namespace)
				{
					cursors.Add(cursor);
				}

				cursor = cursor.SemanticParent;
			}

			StringBuilder sb = new();
			for (int i = cursors.Count - 1; i >= 0; --i)
			{
				sb.Append(cursors[i] + "::");
			}

			if (sb.Length > 2)
			{
				sb.Length -= 2;
			}

			return sb.ToString();
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="cursor"></param>
		/// <param name="qualifedName"></param>
		/// <param name="name"></param>
		/// <param name="namespace"></param>
		/// <returns></returns>
		public static void GetNameInfo(CXCursor cursor, out string qualifedName, out string name, out string @namespace)
		{
			@namespace = GetNamespace(cursor);
			name = cursor.ToString();

			qualifedName = @namespace != "" ? $"{@namespace}::{name}" : name;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="type"></param>
		/// <param name="qualifedName"></param>
		/// <param name="name"></param>
		/// <param name="namespace"></param>
		/// <returns></returns>
		public static void GetNameInfo(CXType type, out string qualifedName, out string name, out string @namespace)
		{
			qualifedName = GetFullyQualifiedType(type.UnqualifiedType);

			CXCursor typeCursor = type.Declaration;
			@namespace = typeCursor.IsInvalid ? "" : GetNamespace(typeCursor);

			if (qualifedName.StartsWith(@namespace))
			{
				int nameStartIndex = @namespace.Length + (@namespace.Length > 0 ? 2 : 0);
				name = qualifedName.Substring(nameStartIndex);
			}
			else
			{
				@namespace = "";
				name = qualifedName;
			}
		}

		/// <summary>
		/// Returns the reflected type kind.
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		public static TypeKind GetTypeKind(CXType type)
		{
			Dictionary<CXTypeKind, TypeKind> clangTypes = new()
			{
					{ CXTypeKind.CXType_Bool,          TypeKind.Bool },
					{ CXTypeKind.CXType_Char_U,        TypeKind.Uint8 },
					{ CXTypeKind.CXType_UChar,         TypeKind.Uint8 },
					{ CXTypeKind.CXType_Char16,        TypeKind.Int16 },
					{ CXTypeKind.CXType_Char32,        TypeKind.Int32 },
					{ CXTypeKind.CXType_UShort,        TypeKind.Uint16 },
					{ CXTypeKind.CXType_UInt,          TypeKind.Uint32 },
					{ CXTypeKind.CXType_ULong,         TypeKind.Uint32 },
					{ CXTypeKind.CXType_ULongLong,     TypeKind.Uint64 },
					{ CXTypeKind.CXType_Char_S,        TypeKind.Int8 },
					{ CXTypeKind.CXType_SChar,         TypeKind.Int8 },
					{ CXTypeKind.CXType_WChar,         TypeKind.Int16 },
					{ CXTypeKind.CXType_Short,         TypeKind.Int16},
					{ CXTypeKind.CXType_Int,           TypeKind.Int32 },
					{ CXTypeKind.CXType_Long,          TypeKind.Int32 },
					{ CXTypeKind.CXType_LongLong,      TypeKind.Int64 },
					{ CXTypeKind.CXType_Float,         TypeKind.Float },
					{ CXTypeKind.CXType_Double,        TypeKind.Double },
					{ CXTypeKind.CXType_LongDouble,    TypeKind.LongDouble },
					{ CXTypeKind.CXType_Void,          TypeKind.Void },
					{ CXTypeKind.CXType_Record,        TypeKind.Class},
					{ CXTypeKind.CXType_Enum,          TypeKind.Enum },
				};

			if (type.TypeClass == CX_TypeClass.CX_TypeClass_TemplateTypeParm)
			{
				return TypeKind.Template;
			}

			if (clangTypes.TryGetValue(type.kind, out var value))
			{
				return value;
			}

			return TypeKind.Invalid;
		}

		/// <summary>
		/// Returns the source location of a cursor.
		/// </summary>
		/// <param name="cursor"></param>
		/// <returns></returns>
		public static SourceLocation GetSourceLocation(CXCursor cursor)
		{
			cursor.Location.GetFileLocation(out var file, out var line, out var _, out _);

			return new SourceLocation()
			{
				Filepath = file.Name.ToString(),
				Line = line
			};
		}

		/// <summary>
		/// Get the class type of a cursor.
		/// </summary>
		/// <param name="cursor"></param>
		/// <returns></returns>
		public static ClassType GetClassType(CXCursor cursor)
		{
			CXCursorKind kind = cursor.Kind;
			if (kind == CXCursorKind.CXCursor_ClassTemplate)
			{
				kind = cursor.TemplateCursorKind;
			}

			return kind == CXCursorKind.CXCursor_StructDecl ? ClassType.Struct : ClassType.Class;
		}
	}
}
