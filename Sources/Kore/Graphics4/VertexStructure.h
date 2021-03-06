#pragma once

namespace Kore {
	namespace Graphics4 {
		enum VertexData {
			NoVertexData,
			Float1VertexData,
			Float2VertexData,
			Float3VertexData,
			Float4VertexData,
			Float4x4VertexData, // not supported in fixed function OpenGL
			Short2NormVertexData,
			Short4NormVertexData,
			ColorVertexData
		};

		enum Usage {
			StaticUsage,
			DynamicUsage, 
			ReadableUsage
		};

		// Fixed-function vertex attributes
		enum VertexAttribute {
			NoVertexAttribute,
			VertexCoord,
			VertexNormal,
			VertexColor0,
			VertexColor1,
			VertexTexCoord0,
			VertexTexCoord1,
			VertexTexCoord2,
			VertexTexCoord3,
			VertexTexCoord4,
			VertexTexCoord5,
			VertexTexCoord6,
			VertexTexCoord7,
		};

		class VertexElement {
		public:
			const char* name;
			VertexAttribute attribute; // for fixed-function (OpenGL 1.x)
			VertexData data;

			VertexElement() : name(nullptr), data(NoVertexData) {}

			VertexElement(const char* name, VertexData data) : name(name), data(data) {}

			VertexElement(VertexAttribute attribute, VertexData data) : name(""), attribute(attribute), data(data) {}
		};

		class VertexStructure {
		public:
			const static int maxElementsCount = 16;
			VertexElement elements[maxElementsCount];
			int size;
			bool instanced;
			u32 id;

			VertexStructure() : size(0), instanced(false) {
				static u32 lastId = 0;
				id = lastId++;
			}

			void add(const char* name, VertexData data) {
				elements[size++] = VertexElement(name, data);
			}

			void add(VertexAttribute attribute, VertexData data) {
				elements[size++] = VertexElement(attribute, data);
			}
		};
	}
}
