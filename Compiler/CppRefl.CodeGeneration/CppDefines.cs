namespace CppRefl.CodeGeneration
{
	// Defines found in our CPP code (see CppReflMarkup.h)
	public static class CppDefines
	{
		/// <summary>
		/// Annotation marker (used by REFLECTED, REFL_TAG, and REFL_ATTR).
		/// </summary>
		public const string Annotation = "cpprefl";
		public const char AnnotationSeparator = ',';
		
		/// <summary>
		/// Preprocessor flag that that is only defined during reflection compilation.
		/// </summary>
		public const string BuildReflection = "CPPREFL_BUILD_REFLECTION()";

		/// <summary>
		/// File id preprocessor flag.
		/// </summary>
		public const string FileId = "CPPREFL_INTERNAL_FILE_ID";

		/// <summary>
		/// String that marks where the GENERATED_REFLECTION_CODE() macro is inside the class definition. We use this to determine which line its on (which lets us do the expansion of the macro).
		/// </summary>
		public const string GeneratedReflectionCodeMarker = "__CppReflGeneratedReflectionCodeMarker";

		/// <summary>
		/// Prefix of the GENERATED_REFLECTION_CODE() body expands to.
		/// </summary>
		public const string InternalReflectionMacroPrefix = "__CPPREFLPRIVATE_REFLECTION_CODE";

		public static class Namespaces
		{
			public const string Public = "cpprefl";
			public const string Private = "CppReflPrivate";
		}
	}
}
