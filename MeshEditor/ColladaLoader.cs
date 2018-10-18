using Assimp;
using SharpDX;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MeshEditor
{
	class ColladaLoader : IModelLoader
	{
		public Model Load(string filename)
		{
			AssimpContext assimpContext = new AssimpContext();

			var scene= assimpContext.ImportFile(filename);

			for (int i = 0; i < scene.MeshCount; i++)
			{
					var mesh = scene.Meshes[i];
				if(mesh.Name == "Box001")
				{
					Model model = new Model(
						mesh.Vertices.Select(w => new Vector3(w.X, w.Y, w.Z)),
						mesh.Normals.Select(w => new Vector3(w.X, w.Y, w.Z)),
						mesh.GetIndices(),
						mesh.Bones
						);
					return model;
				}
			}
			return null;
		}
	}
}
