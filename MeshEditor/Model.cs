using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Assimp;
using SharpDX;
using SharpDX.Direct3D11;
using Buffer = SharpDX.Direct3D11.Buffer;

namespace MeshEditor
{
	class Model
	{
		struct DisplayVertex
		{
			public Vector3 position;
			public Vector4 color;
			public Vector3 normal;
			public Vector4 boneWeights;
			public Int4 noneIndices;
		}

		Buffer vertexBuffer, indexBuffer;

		public Model(Device device, IEnumerable<Vector3> vertices, IEnumerable<Vector3> normals, int[] indices, List<Bone> bones)
		{
			vertexBuffer = new Buffer(device, )))


		}

		internal void Render(Device device, DeviceContext deviceContext)
		{
			throw new NotImplementedException();
		}
	}
}
