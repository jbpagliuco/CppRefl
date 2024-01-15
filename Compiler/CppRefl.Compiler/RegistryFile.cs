using System.Text.Json;
using System.Text.Json.Serialization;
using CppRefl.CodeGeneration.Reflection;

namespace CppRefl.Compiler
{
	public static class RegistryFile
	{
		/// <summary>
		/// Open (and lock) a file.
		/// </summary>
		/// <param name="filename"></param>
		/// <returns></returns>
		/// <exception cref="Exception"></exception>
		private static FileStream OpenFile(string filename)
		{
			for (int numTries = 0; numTries < 1000; numTries++)
			{
				FileStream? fs = null;
				try
				{
					fs = new FileStream(filename, FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.Read);
					return fs;
				}
				catch (IOException)
				{
					if (fs != null)
					{
						fs.Dispose();
					}
					Thread.Sleep(50);
				}
			}

			throw new Exception("Failed to open file: " + filename);
		}

		/// <summary>
		/// Append a registry to a file.
		/// </summary>
		/// <param name="outputRegistryFilename"></param>
		/// <param name="inputFilename"></param>
		/// <param name="registry"></param>
		public static void AppendRegistry(string outputRegistryFilename, string inputFilename, Registry registry)
		{
			using var outputFilestream = OpenFile(outputRegistryFilename);
			using var reader = new StreamReader(outputFilestream);
			string fileContents = reader.ReadToEnd();
			
			Registry outputRegistry = registry;

			if (fileContents != "")
			{
				// Read the existing registry.
				outputRegistry = Registry.FromJson(fileContents)!;

				// Merge the new registry.
				outputRegistry.Merge(registry);
			}

			// Write to the file.
			outputFilestream.Position = 0;
			outputFilestream.SetLength(0);
			outputRegistry.ToJson(outputFilestream);
		}
	}
}
