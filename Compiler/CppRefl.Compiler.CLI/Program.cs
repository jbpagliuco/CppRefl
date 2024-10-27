using CommandLine;
using CommandLine.Text;
using CppRefl.CodeGeneration;
using CppRefl.CodeGeneration.Reflection;
using CppRefl.Compiler;

var commandLine = ParseCommandLine(args);

try
{
	switch (commandLine)
	{
		case FileOptions opts:
			CompileFile(opts);
			break;

		case ModuleOptions opts:
			CompileModule(opts);
			break;

		default:
			throw new NotImplementedException();
	}
}
catch (AggregateException e)
{
	Console.WriteLine("One or more compile errors occurred during reflection generation.");
	foreach (var exc in e.InnerExceptions)
	{
		Console.WriteLine(exc.Message);
		Console.Error.WriteLine(exc.Message);
	}

	return 1;
}
catch (System.Exception e)
{
	Console.WriteLine("One or more compile errors occurred during reflection generation.");
	Console.Error.WriteLine(e.Message);
	return 1;
}

return 0;






static object ParseCommandLine(string[] args)
{
	var verbs = new[] { typeof(FileOptions), typeof(ModuleOptions) };

	var parsedArguments = new Parser(with =>
	{
		with.EnableDashDash = true;
		with.GetoptMode = true;
	}).ParseArguments(args, verbs);

	if (parsedArguments.Errors.Any())
	{
		var builder = SentenceBuilder.Create();
		var errorMessages = HelpText.RenderParsingErrorsTextAsLines(parsedArguments, builder.FormatError, builder.FormatMutuallyExclusiveSetErrors, 1);

		var excList = errorMessages.Select(msg => new ArgumentException(msg)).ToList();

		if (excList.Any())
		{
			throw new AggregateException(excList);
		}
	}

	return parsedArguments.Value;
}

static void CompileFile(FileOptions opts)
{
	var compiler = new Compiler(new CompilerParams()
	{
		InputFile = new(opts.InputFilename),
		ModuleName = opts.ModuleName,
		ModuleDirectory = new(opts.ModuleDirectory),
		ClangArgs = opts.ClangArgs,
		IncludePaths = opts.IncludePaths,
		Definitions = opts.Definitions,
		RaiseClangWarnings = !opts.NoRaiseClangWarnings,
		RaiseClangErrors = !opts.NoRaiseClangErrors,
		OutputDirectory = new(opts.OutputDirectory)
	});

	// Compile the program.
	compiler.GenerateRegistry();

	CodeGeneratorFileParams @params = new()
	{
		InputFilename = new(opts.InputFilename),
		Registry = compiler.Registry,
		ModuleDirectory = new(opts.ModuleDirectory),
		OutputDirectory = new(opts.OutputDirectory)
	};

	var codeGenerator = new CodeGenerator();
	codeGenerator.GenerateFileCode(@params);

	// Cleanup (useful when the given file didn't actually contain any reflected data).
	// compiler.CleanupUnusedFiles();
}

static void CompileModule(ModuleOptions opts)
{
	var moduleRegistry = Registry.CollectFileRegistries(opts.ModuleName, new DirectoryInfo(opts.ModuleDirectory),
		new DirectoryInfo(opts.OutputDirectory));

	// Generate module code.
	CodeGeneratorModuleParams @params = new()
	{
		Registry = moduleRegistry,
		ModuleName = opts.ModuleName,
		ModuleDirectory = new(opts.ModuleDirectory),
		OutputDirectory = new(opts.OutputDirectory)
	};

	var codeGenerator = new CodeGenerator();
	codeGenerator.GenerateModuleCode(@params);
}

public record CommonOptions
{
	[Option(Required = true, HelpText = "Name of the module we're generating reflection for (Game, Engine, Core, etc.)")]
	public string ModuleName { get; init; } = string.Empty;

	[Option(Required = true, HelpText = "Base file path of the module. Any symbols defined outside of this filepath are ignored.")]
	public string ModuleDirectory { get; init; } = string.Empty;

	[Option(Required = true, HelpText = "The directory to generate output files under.")]
	public required string OutputDirectory { get; init; }

	//[Option(HelpText = "List of .DLL files containing external code generators.", Separator = ';')]
	//public IEnumerable<string> CodeGeneratorDlls { get; init; } = Enumerable.Empty<string>();
}

[Verb("file")]
public record FileOptions : CommonOptions
{
	[Option("entry", Required = true, HelpText = "Entrypoint of the program. Should be a .cpp of .h file.")]
	public string InputFilename { get; init; } = string.Empty;

	[Option(Required = false, HelpText = "List of arguments to pass to clang.", Separator = ';')]
	public IEnumerable<string> ClangArgs { get; init; } = Array.Empty<string>();

	[Option(Required = false, HelpText = "List of include paths for the program.", Separator = ';')]
	public IEnumerable<string> IncludePaths { get; init; } = Enumerable.Empty<string>();

	[Option(Required = false, HelpText = "List of preprocessor defines for the program.", Separator = ';')]
	public IEnumerable<string> Definitions { get; init; } = Array.Empty<string>();

	[Option(HelpText = "Don't raise clang warnings during compilation (warings will result in reflection data not being generated).")]
	public bool NoRaiseClangWarnings { get; init; } = false;

	[Option(HelpText = "Don't raise clang errors during compilation (errors will result in reflection data not being generated).")]
	public bool NoRaiseClangErrors { get; init; } = false;
}

[Verb("module")]
public record ModuleOptions : CommonOptions
{ }